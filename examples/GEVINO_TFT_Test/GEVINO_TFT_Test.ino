// Example for GEVINO TFT v1.1
// 18/12/2019   Giorgio Evangelista
// Updated 2026-04-22 : merged 7" and 10" capacitive touch support
//                      into a single sketch, with auto-detection.
//
// The program boots up, performs a self-test of all the peripherals
// on the board (USB flash reader, CAN, Ethernet, RTC, FRAM, analog I/O,
// ESP Wi-Fi, SIM800L GSM, shift-register based digital I/O) and prints
// the outcome both on the TFT and on the Serial USB console.
//
// The GEVINO board can be equipped with two different capacitive touch
// panels, depending on the TFT size:
//   - 7"  panel : FT5206 controller, I2C address 0x38
//   - 10" panel : GT9271 controller, I2C address 0x5D, INT on D38
// The sketch probes both I2C addresses at startup, detects which panel
// is connected, and prints either "Touch 7 ok" or "Touch 10 ok".
// At the end of the boot sequence the main loop draws green circles at
// the touch coordinates, automatically using the driver that matches
// the detected panel.

// Install "Arduino SAMD board"
// Use Arduino Zero Native Port

// install WiFi ESP library github.com/ekstrand/ESP8266wifi
// install eeprom library https://github.com/cmaglie/FlashStorage
// install Adafruit FRAM library https://github.com/adafruit/Adafruit_FRAM_I2C

#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

// ----------------------- RTC DS3231 -----------------------
#include <DS3231.h>
DS3231 clk;
RTCDateTime dt;

// ----------------------- FRAM -----------------------
// For 32 bit FRAM: https://github.com/RobTillaart/FRAM_I2C
// For 16 bit FRAM use the plain "FRAM fram;" class.
#include "FRAM.h"
FRAM32 fram32;
FRAM fram16;

// ----------------------- Ethernet W5500 -----------------------
#include <Ethernet.h>
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 254);

// ----------------------- USB Flash Reader/Writer (CH376) -----------------------
#include <Ch376msc.h>
Ch376msc flashDrive(CH376_CS, CH376_INT); // chipSelect

// ----------------------- 7" Capacitive Touch (FT5206) -----------------------
#include <FT5206.h>
FT5206 cts = FT5206(TouchInt);
uint8_t registers[FT5206_REGISTERS];
uint16_t coordinates[5][2];
uint8_t current_touches = 0;

// ----------------------- 10" Capacitive Touch (GT9271) -----------------------
// The GT9271 does not have a ready-made Arduino library in this project,
// so the low level register access is implemented directly on top of
// Wire.h further down (readGT9271TouchAddr / readGT9271TouchLocation /
// writeGT9271TouchRegister). The code is ported from 10inch_touch.ino.
#define GT9271_ADDR   0x5D   // GT9271 I2C slave address
#define GT9271_INT    38     // GT9271 interrupt line (active low when a touch is reported)

// Simple container used by readGT9271TouchLocation() to return coordinates.
struct TouchLocation
{
  uint16_t x;
  uint16_t y;
};
TouchLocation touchLocations[10];

// Forward declarations for the GT9271 helper functions implemented below.
uint8_t readGT9271TouchLocation( TouchLocation * pLoc, uint8_t num );
void    writeGT9271TouchRegister( uint16_t regAddr, uint8_t *val, uint16_t cnt );
uint8_t readGT9271TouchAddr( uint16_t regAddr, uint8_t * pBuf, uint8_t len );

// State kept between calls of the 10" polling routine.
uint8_t  im10    = 0;    // index of the touch point to pick from the hit list
int      ascissa = 0;    // last touch X coordinate (0..1023), -1 if none
int      ordinata = 0;   // last touch Y coordinate (0..599),  -1 if none

// Flags describing which touch panel has been detected on the I2C bus.
// Only one of the two will normally be set after the probing in setup().
bool touch7Found  = false;  // true if FT5206 (7")  answered on 0x38
bool touch10Found = false;  // true if GT9271 (10") answered on 0x5D

// ----------------------- CAN bus (MCP2515) -----------------------
// Install https://github.com/sandeepmistry/arduino-CAN from Arduino Library Manager
// Change on MCP2515.h
byte MCP2515_DEFAULT_CS_PIN  =  5;
byte MCP2515_DEFAULT_INT_PIN =  -1;
#include <CAN.h>
byte CanOk = 0;


String str;

void setup() {

  // ---- Open every serial port on the board ----
  SerialUSB.begin(115200);
  Serial.begin(115200);     // ESP Wi-Fi
  Serial1.begin(115200);    // RS485
  Serial2.begin(38400);     // BT HC-06-07-08  + RS232
  Serial3.begin(57600);     // GSM SIM800L

  delay(2000);
  //  while(!SerialUSB);
  SerialUSB.println("Begin Test");

  // ---- Board level GPIO / shift-registers initialization ----
  gevino_io_setup();
  if( tft.init() )   SerialUSB.println("RAIO controller OK");
  else               SerialUSB.println("RAIO controller FAIL");

  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  CopySDonFlash();          // If All_76_Pic_65K.bin or font.bin exist on SD, copy them to flash.

  // Draw the "GEVA" logo in blue as a visual "I am alive" marker.
  tft.setFont(&notoSansBold_64pt);
  tft.setTextColor(BLUE);
  tft.setCursor(700,500);
  tft.println("GEVA");
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.setTextColor(WHITE);
  tft.setCursor(0,32);

  // ---- CH376 USB flash reader ----
  flashDrive.init();
  delay(100);
  if (flashDrive.pingDevice()) {
    tft.println("Ch376 ok");
    SerialUSB.println("Ch376 ok");
  } else {
    tft.println("Ch376 fail");
    SerialUSB.println("Ch376 fail");
  }
  if (flashDrive.driveReady()) {
    tft.println("Flash Key ok");
    SerialUSB.println("Flash Key Ok");
  } else {
    tft.println("Flash Key fail");
    SerialUSB.println("Flash Key fail");
  }

  // ---- CAN bus (MCP2515) ----
  if (!CAN.begin(500E3)){
    tft.println("CAN FAIL");
    SerialUSB.println("CAN FAIL");
  } else{
    tft.println("CAN Ok");
    SerialUSB.println("CAN Ok");
    CanOk = 1;
  }

  // ---- Ethernet W5500 ----
  Ethernet.init(ETH_CS);
  if ( Ethernet.begin(mac) ) {
    SerialUSB.println("ETH OK");
    tft.println("ETH ok");
  } else {
    SerialUSB.println("ETH FAIL");
    tft.println("ETH fail");
  }

  // ---- Real Time Clock DS3231 ----
  clk.begin();
  if ( clk.isReady() ) {
    SerialUSB.println("RTC OK");
    tft.println("RTC ok");
  } else {
    SerialUSB.println("RTC FAIL");
    tft.println("RTC fail");
  }

  // -------------------------------------------------------------------
  // ---- Touch panel auto-detection ----
  // Probe both controllers on the I2C bus. The one that replies is the
  // one physically connected to this board. The detected size is shown
  // on the TFT so the operator can verify the hardware assembly.
  // -------------------------------------------------------------------
  Wire.begin();
  Wire.setClock(15000000UL);

  // Probe FT5206 at 0x38 (7" panel).
  Wire.beginTransmission(0x38);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission(0x38);
  Wire.requestFrom(0x38, 1);
  if ( Wire.available() ) {
    touch7Found = true;
    SerialUSB.println("Touch 7\" OK");
    tft.print("Touch 7\" ok  -  ");
  } else {
    SerialUSB.println("Touch 7\" FAIL");
  }

  // Probe GT9271 at 0x5D (10" panel).
  Wire.begin();
  Wire.setClock(15000000UL);
  Wire.beginTransmission(0x5D);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission(0x5D);
  Wire.requestFrom(0x5D, 1);
  if ( Wire.available() ) {
    touch10Found = true;
    SerialUSB.println("Touch 10\" OK");
    tft.print("Touch 10\" ok  -  ");
  } else {
    SerialUSB.println("Touch 10\" FAIL");
  }

  // If neither panel answered, print a single "Touch fail" placeholder
  // so the rest of the boot text keeps its layout.
  if ( !touch7Found && !touch10Found ) {
    tft.print("Touch fail  -  ");
  }

  // Initialize only the controller that was actually detected.
  if ( touch7Found ) {
    cts.begin();                              // FT5206 driver init
  }
  if ( touch10Found ) {
    digitalWrite(GT9271_INT, LOW);            // pull the line low before switching to input
    pinMode     (GT9271_INT, INPUT);          // the GT9271 drives this line on touch events
  }

  // ---- FRAM (32 bit first, fall back to 16 bit) ----
  byte i32 = fram32.begin(0x50);
  byte i16 = fram16.begin(0x50);
  if (!i32){
    tft.println("Found 32bit I2C FRAM");
    SerialUSB.println("Found 32bit I2C FRAM");
  }else if (!i16){
    tft.println("Found 16bit I2C FRAM");
    SerialUSB.println("Found 16bit I2C FRAM");
  }else{
    tft.println("NOT Found I2C FRAM");
    SerialUSB.println("Not Found I2C FRAM");
  }

  // ---- TFT backlight sweep: 100% -> 0% -> 100% (visual check) ----
  for ( int i = 100; i > 0; i--) {
    delay(15);
    tft.PWM0(1, 3, 100, 100, i);     // 0..100% backlight
  }
  for ( int i = 0; i <= 100; i++) {
    delay(15);
    tft.PWM0(1, 3, 100, 100, i);     // 0..100% backlight
  }

  // ---- Digital outputs / inputs (shift register loop-back) test ----
  testLeds();

  // ---- Analog Out on A0, Analog In on A1 / A2 / A3 ----
  //  pinMode(A0, OUTPUT);       // Do not declare it: it will stop working as a DAC.
  DAC->CTRLB.bit.REFSEL = 0;  // Use the internal 1V reference as DAC output reference.
  analogWrite(A0, 0);         // Preset the DAC to 0 V.

  int tmp;

  // DAC = 0 V
  SerialUSB.print("DAC=0V   A1=");
  tft.print("DAC=0V   A1=");
  delay(100);
  tmp = analogRead(A1);
  SerialUSB.print(tmp);
  tft.print(tmp);
  SerialUSB.print("   A2=");
  tft.print("   A2=");
  tmp = analogRead(A2);
  SerialUSB.print(tmp);
  tft.print(tmp);
  SerialUSB.print("   A3=");
  tft.print("   A3=");
  tmp = analogRead(A3);
  SerialUSB.println(tmp);
  tft.println(tmp);

  // DAC = ~5 V (mid scale of the 1V reference x output stage gain)
  analogWrite(A0, 512);
  SerialUSB.print("DAC=5V   A1=");
  tft.print("DAC=5V   A1=");
  delay(100);
  tmp = analogRead(A1);
  SerialUSB.print(tmp);
  tft.print(tmp);
  SerialUSB.print("  A2=");
  tft.print("  A2=");
  tmp = analogRead(A2);
  SerialUSB.print(tmp);
  tft.print(tmp);
  SerialUSB.print("  A3=");
  tft.print("  A3=");
  tmp = analogRead(A3);
  SerialUSB.println(tmp);
  tft.println(tmp);

  // DAC = ~10 V (full scale)
  analogWrite(A0, 1023);
  SerialUSB.print("DAC=10V  A1=");
  tft.print("DAC=10V  A1=");
  delay(100);
  tmp = analogRead(A1);
  SerialUSB.print(tmp);
  tft.print(tmp);
  SerialUSB.print(" A2=");
  tft.print(" A2=");
  tmp = analogRead(A2);
  SerialUSB.print(tmp);
  tft.print(tmp);
  SerialUSB.print(" A3=");
  tft.print(" A3=");
  tmp = analogRead(A3);
  SerialUSB.println(tmp);
  tft.println(tmp);

  // ---- ESP Wi-Fi module ----
  // Ask the ESP its firmware version and display the answer.
  Serial.println("AT+GMR");
  str = Serial.readStringUntil('\n');
  tft.setCursor(600, 60);
  tft.print("ESP ");
  tft.print(str);
  SerialUSB.println(str);
  str = Serial.readStringUntil('\n');
  tft.setCursor(600, 90);
  tft.print(str);
  SerialUSB.println(str);
  str = Serial.readStringUntil('\n');
  tft.setCursor(600, 120);
  tft.print(str);
  SerialUSB.println(str);


  // ---- SIM800L GSM module ----
  // Same check, on the GSM modem.
  Serial3.println("AT+GMR");
  str = Serial3.readStringUntil('\n');
  tft.setCursor(600, 150);
  tft.print("SIM800L ");
  tft.print(str);
  SerialUSB.println(str);
  str = Serial3.readStringUntil('\n');
  tft.setCursor(600, 180);
  tft.print("SIM800L ");
  tft.print(str);
  SerialUSB.println(str);

}

void loop() {
  TestTouch();  // Draw a green dot wherever the user touches the screen.
  updateIO();   // Refresh shift register based digital I/O (reads inputs, writes outputs).

  // Mirror the 12 digital inputs to the 8 digital outputs so the operator
  // can manually verify the I/O chain by flipping switches on the test jig.
  if( In0 || In8  ) setOut0;
  else resOut0;

  if( In1 || In9  ) setOut1;
  else resOut1;

  if( In2 || In10 ) setOut2;
  else resOut2;

  if( In3 || In11 ) setOut3;
  else resOut3;

  if( In4 ) setOut4;
  else resOut4;

  if( In5 ) setOut5;
  else resOut5;

  if( In6 ) setOut6;
  else resOut6;

  if( In7 ) setOut7;
  else resOut7;

  byte b;

  // ---- Heartbeat on USB / RS485 ----
  if(SerialUSB) {
    static unsigned long ms;
    if (millis() - ms >= 1000)
    {
      ms = millis();
      SerialUSB.print("I'm happy \n");
      SerialUSB.println(ms);

      set_Rs485_TxEn;
      Serial1.print("I'm happy ");         // Send on RS485
    }
  }

  // ---- Serial port bridge: forward data between USB and the other ports ----
  // Wi-Fi ESP
  if ( Serial.available() )   SerialUSB.write(Serial.read());

  // RS485 (release the driver when the last byte left the shift register)
  if ( Serial1.available() )   SerialUSB.write(Serial1.read());
  if (SERCOM0->USART.INTFLAG.bit.TXC)
  {
    res_Rs485_TxEn;
    SERCOM0->USART.INTFLAG.bit.TXC = 1;
  }

  // BT + RS232
  if ( Serial2.available() )   SerialUSB.write(Serial2.read());

  // SIM800L GSM
  if ( Serial3.available() )   SerialUSB.write(Serial3.read());

  // Forward any byte coming from USB out to every other serial port.
  if ( SerialUSB.available() ) {
    b = SerialUSB.read();
    set_Rs485_TxEn;
    Serial.write(b);
    Serial1.write(b);
    Serial2.write(b);
    Serial3.write(b);
  }
}


// testLeds: drives each output one at a time and verifies that the paired
// input reports the expected state. The shift-register based I/O is
// looped back on the test jig so output N must light up input N (and a
// second input on some channels). Every step also prints a "-" for each
// active input and a "_" for each inactive one, building a visual bar.
void testLeds( void ) {
  setOut0; resOut1; resOut2; resOut3; resOut4; resOut5; resOut6; resOut7;
  updateIO();
  delay(250);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( !In0 || In1 || In2 || In3 || In4 || In5 || In6 || In7 || In8 || In9 || In10 || In11 ) {
    tft.println("o0 i0 fail");
    SerialUSB.println("o0 i0 fail");
  } else {
    tft.println("o0 i0 ok");
    SerialUSB.println("o0 i0 ok");
  }
  delay(250);
  resOut0;
  setOut1;
  updateIO();
  delay(250);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( In0 || !In1 || In2 || In3 || In4 || In5 || In6 || In7 || In8 || In9 || In10 || In11 ) {
    tft.println("o1 i1 fail");
    SerialUSB.println("o1 i1 fail");
  } else {
    tft.println("o1 i1 ok");
    SerialUSB.println("o1 i1 ok");
  }
  delay(250);
  resOut1;
  setOut2;
  updateIO();
  delay(250);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( In0 || In1 || !In2 || In3 || In4 || In5 || !In6 || In7 || In8 || In9 || In10 || In11 ) {
    tft.println("o2 i2 i6 fail");
    SerialUSB.println("o2 i2 i6 fail");
  } else {
    tft.println("o2 i2 i6 ok");
    SerialUSB.println("o2 i2 i6 ok");
  }
  delay(250);
  resOut2;
  setOut3;
  updateIO();
  delay(250);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( In0 || In1 || In2 || !In3 || In4 || In5 || In6 || !In7 || In8 || In9 || In10 || In11 ) {
    tft.println("o3 i3 i7 fail");
    SerialUSB.println("o3 i3 i7 fail");
  } else {
    tft.println("o3 i3 i7 ok");
    SerialUSB.println("o3 i3 i7 ok");
  }
  delay(250);
  resOut3;
  setOut4;
  updateIO();
  delay(250);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( In0 || In1 || In2 || In3 || !In4 || In5 || In6 || In7 || !In8 || In9 || In10 || In11 ) {
    tft.println("o4 i4 i8 fail");
    SerialUSB.println("o4 i4 i8 fail");
  } else {
    tft.println("o4 i4 i8 ok");
    SerialUSB.println("o4 i4 i8 ok");
  }
  delay(250);
  resOut4;
  setOut5;
  updateIO();
  delay(250);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( In0 || In1 || In2 || In3 || In4 || !In5 || In6 || In7 || In8 || !In9 || In10 || In11 ) {
    tft.println("o5 i5 i9 fail");
    SerialUSB.println("o5 i5 i9 fail");
  } else {
    tft.println("o5 i5 i9 ok");
    SerialUSB.println("o5 i5 i9 ok");
  }
  delay(250);
  resOut5;
  setOut6;
  updateIO();
  delay(250);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( In0 || In1 || In2 || In3 || In4 || In5 || In6 || In7 || In8 || In9 || !In10 || In11 ) {
    tft.println("o6 i10 fail");
    SerialUSB.println("o6 i10 fail");
  } else {
    tft.println("o6 i10 ok");
    SerialUSB.println("o6 i10 ok");
  }
  delay(250);
  resOut6;
  setOut7;
  updateIO();
  delay(250);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( In0 || In1 || In2 || In3 || In4 || In5 || In6 || In7 || In8 || In9 || In10 || !In11 ) {
    tft.println("o7 i11 fail");
    SerialUSB.println("o7 i11 fail");
  } else {
    tft.println("o7 i11 ok");
    SerialUSB.println("o7 i11 ok");
  }
  delay(250);

  // Turn every output on at the same time and check that every input
  // reacts. Also light up the on-board LED for the marketing photos.
  setOut0; setOut1; setOut2; setOut3; setOut4; setOut5; setOut6; setOut7;
  updateIO();
  setLed;
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  delay(2000);
  //  updateIO();   // A single pin is shared for LatchIn and LatchOut
  updateIO();
  Level();
  if ( !In0 || !In1 || !In2 || !In3 || !In4 || !In5 || !In6 || !In7 || !In8 || !In9 || !In10 || !In11 ) {
    tft.println("all on fail");
    SerialUSB.println("all on fail");
  } else {
    tft.println("all on ok");
    SerialUSB.println("all on ok");
  }
  delay(250);
}

// Level: print a small bar showing the current state of the 12 digital
// inputs. An active input is drawn as "-", an inactive one as "_".
void Level (void) {
  if ( In0 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In1 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In2 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In3 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In4 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In5 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In6 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In7 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In8 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In9 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In10 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  if ( In11 ) {
    tft.print("-");
    SerialUSB.print("-");
  } else {
    tft.print("_");
    SerialUSB.print("_");
  }
  tft.print("  ");
  SerialUSB.print("  ");
}

// TestTouch: polls the touch controller that was detected at boot and
// draws a small green filled circle at the touched position. The screen
// is 1024 x 600 pixels. Both controllers are oriented the same way from
// the user point of view after the X/Y flip below.
void TestTouch ( void) {

  if ( touch7Found ) {
    // ---------- 7" panel, FT5206 ----------
    cts.setTouchLimit(1);              // Only track one finger at a time.
    if (cts.touched()) {
      uint16_t x, y;
      cts.getTSregisters(registers);
      current_touches = cts.getTScoordinates(coordinates, registers);
      if (current_touches < 1) return;

      x = coordinates[0][0];
      y = coordinates[0][1];
      // The panel is mounted rotated 180 deg vs. the display memory,
      // so flip both axes to match the on-screen pixel coordinates.
      tft.fillCircle(1024 - x, 600 - y, 5, RGB565(0, 255, 0));
    }
  }
  else if ( touch10Found ) {
    // ---------- 10" panel, GT9271 ----------
    int tx = -1;
    int ty = -1;
    tocco(&tx, &ty);                   // Read one fresh sample from GT9271.
    // tocco() already flips the axes and returns -1 when no new touch
    // is present, so we only need a sanity check on the window.
    if (tx > 0 && tx < 1024 && ty > 0 && ty < 600) {
      tft.fillCircle(tx, ty, 5, RGB565(0, 255, 0));
    }
  }
}


// -------------------------------------------------------------------
//               GT9271 low level driver (10" panel)
// Ported from 10inch_touch.ino. These routines talk to the controller
// on the I2C bus and convert its 16 bit register stream into pixel
// coordinates. The INT line on GT9271_INT goes low when the controller
// has a new touch event ready to be read.
// -------------------------------------------------------------------

// tocco: read one sample from the GT9271 when the INT line signals a
// new touch. Returns the already rotated pixel coordinates through *x
// and *y, or -1/-1 if nothing new happened since the last call.
void tocco (int *x, int *y)
{
  *x = -1;
  *y = -1;

  uint8_t flag = 1;
  static int16_t oldx;
  static int16_t oldy;

  if( !digitalRead(GT9271_INT) )
  {
    if( flag ){
      flag = 0;

      uint8_t count = readGT9271TouchLocation( touchLocations, 10 );
      (void)count;

      // Reject duplicates: the controller keeps reasserting INT while
      // the finger is still on the glass, so only report genuine moves.
      if (oldx != (1024 - touchLocations[im10].x) || oldy != (600 - touchLocations[im10].y)) {
        *x   = 1024 - touchLocations[im10].x;
        *y   = 600  - touchLocations[im10].y;
        oldx = 1024 - touchLocations[im10].x;
        oldy = 600  - touchLocations[im10].y;
      }
      else {
        *x = -1;
        *y = -1;
      }
    }
  } else {
    flag = 1;
  }
}

// writeGT9271TouchRegister: raw I2C write of "cnt" bytes into the
// 16 bit register address "regAddr" of the GT9271.
void writeGT9271TouchRegister( uint16_t regAddr, uint8_t *val, uint16_t cnt)
{
  uint16_t i = 0;
  Wire.beginTransmission(GT9271_ADDR);
  Wire.write( regAddr >> 8 );   // register high byte
  Wire.write( regAddr      );   // register low  byte
  for (i = 0; i < cnt; i++, val++)
  {
    Wire.write( *val );         // payload
  }
  uint8_t retVal = Wire.endTransmission();
  (void)retVal;
}

// readGT9271TouchAddr: raw I2C read of "len" bytes starting at the
// 16 bit register address "regAddr" of the GT9271. Returns the number
// of bytes that were actually copied into pBuf.
uint8_t readGT9271TouchAddr( uint16_t regAddr, uint8_t * pBuf, uint8_t len )
{
  Wire.beginTransmission(GT9271_ADDR);
  Wire.write( regAddr >> 8 );   // register high byte
  Wire.write( regAddr      );   // register low  byte
  uint8_t retVal = Wire.endTransmission();
  (void)retVal;

  Wire.requestFrom(GT9271_ADDR, len);

  uint8_t i;
  for (i = 0; (i < len) && Wire.available(); i++)
  {
    pBuf[i] = Wire.read();
  }

  return i;
}

// readGT9271TouchLocation: decode the status register at 0x814E and,
// if at least one finger is present, read the coordinate block that
// starts at 0x8150. Supports up to 10 simultaneous touches (GT9271 can
// do that many, although TestTouch only uses the first one). Finally
// clears the status register so the controller is ready for the next
// frame.
uint8_t readGT9271TouchLocation( TouchLocation * pLoc, uint8_t num )
{
  uint8_t retVal = 0;
  uint8_t i;
  uint8_t k;
  uint8_t ss[1];

  // The loop runs at most once: it is written as a for() just so we can
  // use 'break' as a structured early-exit on error cases.
  for (uint8_t conteggio = 0; conteggio < 1; conteggio++)
  {
    if (!pLoc) break;   // need a destination buffer
    if (!num)  break;   // caller must accept at least one point

    ss[0] = 0;
    readGT9271TouchAddr( 0x814e, ss, 1);
    uint8_t status = ss[0];

    if ((status & 0x0f) == 0) break;      // no fingers on the screen
    uint8_t hitPoints = status & 0x0f;    // number of simultaneous touches

    uint8_t tbuf[32]; uint8_t tbuf1[32]; uint8_t tbuf2[16];
    readGT9271TouchAddr( 0x8150, tbuf, 32);
    /* Uncomment if more than 4 simultaneous touches are required:
    readGT9271TouchAddr( 0x8150 + 32, tbuf1, 32);
    readGT9271TouchAddr( 0x8150 + 64, tbuf2, 16);
    */

    if (hitPoints <= 4)
    {
      // Fast path used by TestTouch: only the first touch point is
      // needed, so we decode just the 4 bytes we care about.
      k = 0;
      pLoc[k].x = tbuf[1] << 8 | tbuf[0];
      pLoc[k].y = tbuf[3] << 8 | tbuf[2];
      retVal = hitPoints;
      break;
    }
    if (hitPoints > 4)
    {
      for (k = 0, i = 0; (i <  4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf[i + 1] << 8 | tbuf[i + 0];
        pLoc[k].y = tbuf[i + 3] << 8 | tbuf[i + 2];
      }

      for (k = 4, i = 0; (i <  4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf1[i + 1] << 8 | tbuf1[i + 0];
        pLoc[k].y = tbuf1[i + 3] << 8 | tbuf1[i + 2];
      }
    }

    if (hitPoints > 8)
    {
      for (k = 0, i = 0; (i <  4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf[i + 1] << 8 | tbuf[i + 0];
        pLoc[k].y = tbuf[i + 3] << 8 | tbuf[i + 2];
      }

      for (k = 4, i = 0; (i <  4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf1[i + 1] << 8 | tbuf1[i + 0];
        pLoc[k].y = tbuf1[i + 3] << 8 | tbuf1[i + 2];
      }

      for (k = 8, i = 0; (i <  2 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf2[i + 1] << 8 | tbuf2[i + 0];
        pLoc[k].y = tbuf2[i + 3] << 8 | tbuf2[i + 2];
      }
    }

    retVal = hitPoints;
    break;
  }

  // Clear the status register so that the controller will assert INT
  // again only on the next touch event.
  ss[0] = 0;
  writeGT9271TouchRegister( 0x814e, ss, 1);

  return retVal;
}
