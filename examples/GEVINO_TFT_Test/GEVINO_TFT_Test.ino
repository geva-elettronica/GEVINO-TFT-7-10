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
// Both are handled by the library (gevino_tft_touch.h): a single call to
// gevino_touch_setup() probes both addresses and selects the driver that
// matches the panel, so this sketch just prints the detected size and
// draws green circles wherever gevino_touch_read() reports a touch.

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

// ----------------------- Capacitive Touch (7" FT5206 / 10" GT9271) ----------
// Both controllers are handled by the library: gevino_touch_setup() probes the
// I2C bus, and the reading functions then work the same way on either panel.
#include "gevino_tft_touch.h"

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
  // Probe the SPI link FIRST, before tft.init(). This is important: if the
  // controller is dead, init() spins forever inside waitWriteFifo()/clearScreen()
  // (readStatus() keeps returning 0xFF, bit7 never clears) and we would never
  // get any further output - that is why only "Begin Test" was showing up.
  tftSpiDiagnose();

  if( tft.init() )   SerialUSB.println("RAIO controller OK");
  else               SerialUSB.println("RAIO controller FAIL");

  tft.selectInternalFont(RA8876_FONT_SIZE_32);

  // SD card check only. The font/image copy to flash is now done automatically
  // at the end of tft.init() (RA8876::copySDtoFlash), so the test just reports
  // whether the SD is readable.
  if ( sd.begin(12, SD_SCK_MHZ(12)) ) {
    tft.println("SD ok");
    SerialUSB.println("SD ok");
  } else {
    tft.println("SD fail");
    SerialUSB.println("SD fail");
  }

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
  // The library probes both controllers on the I2C bus and initialises the
  // one that replies - that is the panel physically fitted to this board.
  // The detected size is shown on the TFT so the operator can verify the
  // hardware assembly.
  // -------------------------------------------------------------------
  if ( gevino_touch_setup() ) {
    SerialUSB.print("Touch ");   SerialUSB.print(gevino_touch_name());   SerialUSB.println(" OK");
    tft.print("Touch ");         tft.print(gevino_touch_name());         tft.print(" ok  -  ");
  } else {
    SerialUSB.println("Touch FAIL");
    tft.print("Touch fail  -  ");
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


// tftSpiDiagnose: run on every boot, BEFORE tft.init(), to report on the USB
// console whether the controller actually answers on SPI. It must run before
// init() because a dead controller makes init() hang forever in waitWriteFifo()
// (readStatus() stuck at 0xFF) - so this is the only place guaranteed to print.
// It does not rely on the display being initialized: it talks to the RA8876
// directly over SPI and tries to distinguish three cases:
//   1) no SPI answer at all  -> wiring / power / reset / CS problem
//   2) SPI answers, register write/read-back works -> chip alive, the dark
//      screen is downstream (PLL, SDRAM, backlight, panel timing)
//   3) flaky / partial echo   -> marginal SPI (speed too high, long wires)
//
// All RA8876 members are public, so we drive the low level read/write helpers
// directly. We set our own SPISettings here instead of trusting tft.m_spiSettings,
// because init() may have bailed out before assigning it.
void tftSpiDiagnose(void) {
  SerialUSB.println();
  SerialUSB.println("==== RA8876 SPI diagnosis ====");
  SerialUSB.print  ("CS=D");          SerialUSB.print(tft.m_csPin);
  SerialUSB.print  ("  RESET=");      SerialUSB.print(tft.m_resetPin);
  SerialUSB.println("  (SPI mode 3, MSB first, 12 MHz)");

  // Make sure the bus is up and CS is idle-high, regardless of how far init() got.
  pinMode(tft.m_csPin, OUTPUT);
  digitalWrite(tft.m_csPin, HIGH);
  SPI.begin();
  SPISettings diag = SPISettings(RA8876_SPI_SPEED, MSBFIRST, SPI_MODE3);
  SPI.beginTransaction(diag);

  // ---- 1) Status register: read it several times. -------------------------
  // The RA8876 status byte must NOT be a constant 0x00 or 0xFF. A stuck value
  // means MISO is never driven, i.e. the controller is not answering.
  SerialUSB.print("Status reg x8: ");
  uint8_t orAll = 0x00, andAll = 0xFF;
  for (int i = 0; i < 8; i++) {
    uint8_t s = tft.readStatus();
    orAll  |= s;
    andAll &= s;
    if (s < 0x10) SerialUSB.print('0');
    SerialUSB.print(s, HEX);
    SerialUSB.print(' ');
  }
  SerialUSB.println();
  if (orAll == 0x00)
    SerialUSB.println(" -> MISO stuck LOW: no answer. Check 3V3, RESET, CS=D42, MISO and SCK wiring.");
  else if (andAll == 0xFF)
    SerialUSB.println(" -> MISO stuck HIGH: line floating / not driven by the controller.");
  else
    SerialUSB.println(" -> Status bits change: the SPI read path looks alive.");

  // ---- 2) Write / read-back on a harmless R/W register --------------------
  // Foreground colour R (0xD2) is a plain 8-bit R/W register with no side
  // effect on a dark panel. If the chip echoes every pattern back, then CS +
  // SCK + MOSI + MISO and the register file are all working.
  const uint8_t pat[] = { 0xA5, 0x5A, 0x00, 0xFF, 0x3C };
  uint8_t saved = tft.readReg(RA8876_REG_FGCR);
  int okCount = 0;
  for (uint8_t i = 0; i < sizeof(pat); i++) {
    tft.writeReg(RA8876_REG_FGCR, pat[i]);
    uint8_t rb = tft.readReg(RA8876_REG_FGCR);
    SerialUSB.print("  FGCR wrote 0x");
    if (pat[i] < 0x10) SerialUSB.print('0');
    SerialUSB.print(pat[i], HEX);
    SerialUSB.print(" read 0x");
    if (rb < 0x10) SerialUSB.print('0');
    SerialUSB.print(rb, HEX);
    if (rb == pat[i]) { SerialUSB.println("  OK"); okCount++; }
    else                SerialUSB.println("  MISMATCH");
  }
  tft.writeReg(RA8876_REG_FGCR, saved);   // restore the original value

  if (okCount == (int)sizeof(pat))
    SerialUSB.println(" -> Register R/W OK: RA8876 responds on SPI. If the panel is still dark,");
  else if (okCount == 0)
    SerialUSB.println(" -> No pattern echoed: SPI link broken or chip held in reset.");
  else
    SerialUSB.println(" -> Partial echo: marginal SPI - lower RA8876_SPI_SPEED or check wire length.");

  // ---- 3) A couple of config registers as a coarse "is the core alive" probe.
  SerialUSB.print  ("CCR=0x");  SerialUSB.print(tft.readReg(RA8876_REG_CCR), HEX);
  SerialUSB.print  ("  MACR=0x"); SerialUSB.print(tft.readReg(RA8876_REG_MACR), HEX);
  SerialUSB.print  ("  DPCR=0x"); SerialUSB.println(tft.readReg(RA8876_REG_DPCR), HEX);

  if (okCount == (int)sizeof(pat))
    SerialUSB.println("    suspect PLL/SDRAM/backlight/panel timing, not the SPI bus.");

  SPI.endTransaction();
  SerialUSB.println("==============================");
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

// TestTouch: polls the touch panel detected at boot and draws a small
// green filled circle at the touched position. The screen is 1024 x 600
// pixels; the library already returns screen coordinates (both panels are
// mounted rotated 180 deg, so it flips the axes for us) so nothing else
// has to change between a 7" and a 10" board.
void TestTouch ( void) {
  int16_t x, y;
  if ( gevino_touch_read(&x, &y) ) {
    tft.fillCircle(x, y, 5, RGB565(0, 255, 0));
  }
}
