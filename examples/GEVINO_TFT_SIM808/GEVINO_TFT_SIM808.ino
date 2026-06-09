// Example for GEVINO TFT v1.1
// 18/12/2019   Giorgio Evangelista

// Install "Arduino SAMD board"
// Use Arduino Zero Native Port

// install WiFi ESP library github.com/ekstrand/ESP8266wifi
// install eeprom library https://github.com/cmaglie/FlashStorage
// install Adafruit FRAM library https://github.com/adafruit/Adafruit_FRAM_I2C

#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

// RTC
#include <DS3231.h>
DS3231 clk;
RTCDateTime dt;

#include "Adafruit_FRAM_I2C.h"
Adafruit_FRAM_I2C fram     = Adafruit_FRAM_I2C();

// W5500
#include <Ethernet.h>
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 254);

//// USB Flash Resader Writer
#include <Ch376msc.h>
Ch376msc flashDrive(CH376_CS, CH376_INT); // chipSelect

// Touch
#include <FT5206.h>
#define CTP_INT          38    // touch data ready for read from FT5206 touch controller
FT5206 cts = FT5206(CTP_INT);
uint8_t registers[FT5206_REGISTERS];
uint16_t coordinates[5][2];
uint8_t current_touches = 0;


// Install https://github.com/sandeepmistry/arduino-CAN/Also from Arduino Library Manager
// Change on MCP2515.h
byte MCP2515_DEFAULT_CS_PIN  =  5;
byte MCP2515_DEFAULT_INT_PIN =  -1;
#include <CAN.h>
byte CanOk = 0;

#define __APN "AT+SAPBR=3,1,\"APN\",\"apn.fastweb.it\""

String str;

void setup() {

  SerialUSB.begin(115200);
  Serial.begin(115200);     // ESP Wi-Fi
  Serial1.begin(115200);    // RS485
  Serial2.begin(38400);     // BT HC-06-07-08  + RS232
  Serial3.begin(9600);      // GSM SIM808

  delay(2000);
  //  while(!SerialUSB);
  SerialUSB.println("Begin Test");

  gevino_io_setup();
  tft.init();
  tft.selectInternalFont(RA8876_FONT_SIZE_32);  
  CopySDonFlash();          // if exist All_76_Pic_65K.bin or font.bin copy on flash

  tft.setFont(&notoSansBold_64pt);
  tft.setTextColor(BLUE);  // Clear Background
  tft.setCursor(700,500);
  tft.println("GEVA");
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.setTextColor(WHITE);  // Clear Background
  tft.setCursor(0,32);

  // Test Flash USB Writer
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

  // Test Can
  if (!CAN.begin(500E3)){
    tft.println("CAN FAIL");
    SerialUSB.println("CAN FAIL");
  } else{
    tft.println("CAN Ok");
    SerialUSB.println("CAN Ok");
    CanOk = 1;
  }

  // W5500
  Ethernet.init(ETH_CS);
  if ( Ethernet.begin(mac) ) {
    SerialUSB.println("ETH OK");
    tft.println("ETH ok");
  } else {
    SerialUSB.println("ETH FAIL");
    tft.println("ETH fail");
  }

  // Test RTC
  clk.begin();
  if ( clk.isReady() ) {
    SerialUSB.println("RTC OK");
    tft.println("RTC ok");
  } else {
    SerialUSB.println("RTC FAIL");
    tft.println("RTC fail");
  }

  // Test Touch
  Wire.begin();
  Wire.setClock(15000000UL);
  Wire.beginTransmission(0x38);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission(0x38);
  Wire.requestFrom(0x38, 1);

  if ( Wire.available() ) {
    SerialUSB.println("Touch OK");
    tft.print("Touch ok  -  ");
  } else {
    SerialUSB.println("Touch FAIL");
    tft.print("Touch fail  -  ");
  }
  cts.begin();

  // Test Fram
  if (fram.begin()) tft.println("Found I2C FRAM");
  else tft.println("NOT Found I2C FRAM");

  for ( int i = 100; i > 0; i--) {  // TFT BackLight Ramp, 100% to 0%
    delay(15);
    tft.PWM0(1, 3, 100, 100, i);     // 0 to 100% Backlight
  }
  for ( int i = 0; i <= 100; i++) {  // TFT BackLight Ramp, 0% to 100%
    delay(15);
    tft.PWM0(1, 3, 100, 100, i);     // 0 to 100% Backlight
  }

  //testLeds();
//
//  // Analog Out on Pin A0
//  //  pinMode(A0, OUTPUT);       // Not declare that it will not work
//  DAC->CTRLB.bit.REFSEL = 0;  // Set Up 1V as DAC Output reference.
//  analogWrite(A0, 0);         // Initialize Dac  to Zero
//
//  int tmp;
//  // Analog Out and IN test
//  SerialUSB.print("DAC=0V   A1=");
//  tft.print("DAC=0V   A1=");
//  delay(100);
//  tmp = analogRead(A1);
//  SerialUSB.print(tmp);
//  tft.print(tmp);
//  SerialUSB.print("   A2=");
//  tft.print("   A2=");
//  tmp = analogRead(A2);
//  SerialUSB.print(tmp);
//  tft.print(tmp);
//  SerialUSB.print("   A3=");
//  tft.print("   A3=");
//  tmp = analogRead(A3);
//  SerialUSB.println(tmp);
//  tft.println(tmp);
//
//  analogWrite(A0, 512);         // Initialize Dac  to Zero
//  SerialUSB.print("DAC=5V   A1=");
//  tft.print("DAC=5V   A1=");
//  delay(100);
//  tmp = analogRead(A1);
//  SerialUSB.print(tmp);
//  tft.print(tmp);
//  SerialUSB.print("  A2=");
//  tft.print("  A2=");
//  tmp = analogRead(A2);
//  SerialUSB.print(tmp);
//  tft.print(tmp);
//  SerialUSB.print("  A3=");
//  tft.print("  A3=");
//  tmp = analogRead(A3);
//  SerialUSB.println(tmp);
//  tft.println(tmp);
//
//  analogWrite(A0, 1023);         // Initialize Dac  to Zero
//  SerialUSB.print("DAC=10V  A1=");
//  tft.print("DAC=10V  A1=");
//  delay(100);
//  tmp = analogRead(A1);
//  SerialUSB.print(tmp);
//  tft.print(tmp);
//  SerialUSB.print(" A2=");
//  tft.print(" A2=");
//  tmp = analogRead(A2);
//  SerialUSB.print(tmp);
//  tft.print(tmp);
//  SerialUSB.print(" A3=");
//  tft.print(" A3=");
//  tmp = analogRead(A3);
//  SerialUSB.println(tmp);
//  tft.println(tmp);

  // Test ESP
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


//  // Test SIM800L
//  Serial3.println("AT+GMR");
//  str = Serial3.readStringUntil('\n');
//  tft.setCursor(600, 150);
//  tft.print("SIM800L ");
//  tft.print(str);
//  SerialUSB.println(str);
//  str = Serial3.readStringUntil('\n');
//  tft.setCursor(600, 180);
//  tft.print("SIM800L ");
//  tft.print(str);
//  SerialUSB.println(str);


  // Change baud at SIM808
  SerialUSB.println("Test SIM808");
  Serial3.println("AT");
  delay(200);
  SerialUSB.println( Serial3.readString());
  Serial3.println("AT+IPR=0");
  delay(200);
  SerialUSB.println( Serial3.readString());
  Serial3.println("AT&W");
  delay(200);
  SerialUSB.println( Serial3.readString());
  Serial3.println("AT+CGPSPWR=1");   // Accendo GPS
  delay(200);
  SerialUSB.println( Serial3.readString());
  Serial3.println("AT+CPIN=?");      // Controllo Sim e Pin
  delay(200);
  SerialUSB.println( Serial3.readString());
  SerialUSB.println("0-98 segnale, 0.7 As RXQUAL values");
  Serial3.println("AT+CSQ");         // Segnale
  delay(200);
  SerialUSB.println( Serial3.readString());

  sendMail();

}

void loop() {
  TestTouch();  // Touch Test
  updateIO();	// Refresh IO read and write (ShiftRegister)

    if( In0 || In8 ) setOut0;
    else resOut0;
  
    if( In1 || In9  ) setOut1;
    else resOut1;
  
    if( In2 || In10  ) setOut2;
    else resOut2;
  
    if( In3 || In11  ) setOut3;
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
  // Serials
    if(SerialUSB) {
      static unsigned long ms;
      static byte ff;
      if (millis() - ms >= 1000)
      {
        ms = millis();
  
    	  set_Rs485_TxEn;
    	  Serial1.print("I'm happy ");		// Send at RS485

      if(ff){
        Serial3.println("AT+CGPSINF=0");
        ff=0;
      }else{
        Serial3.println("AT+CGPSSTATUS?");
        ff=1;
      } 
            
      }
    }

  // Wi-Fi ESP
  if ( Serial.available() )   SerialUSB.write(Serial.read());

  // RS485
  if ( Serial1.available() )   SerialUSB.write(Serial1.read());
  if (SERCOM0->USART.INTFLAG.bit.TXC)   //Remove Tx Enable when the last character has been sent out.
  {
    res_Rs485_TxEn;
    SERCOM0->USART.INTFLAG.bit.TXC = 1;
  }

  // BT + RS232
    if ( Serial2.available() ){
      SerialUSB.print("RS232 -> ");
      SerialUSB.println( Serial2.readString());
    }

  // SIM800L GSM
    if ( Serial3.available() ){  SerialUSB.println( Serial3.readString()); 
  }
  

  // All
  if ( SerialUSB.available() ) {
    b = SerialUSB.read();
    set_Rs485_TxEn;
    Serial.write(b);
    Serial1.write(b);
    Serial2.write(b);
    Serial3.write(b);
  }
}


void testLeds( void ) {      // Turns on most of the LEDs for photos
  setOut0; resOut1; resOut2; resOut3; resOut4; resOut5; resOut6; resOut7;
  updateIO();
  delay(250);
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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
  setOut0; setOut1; setOut2; setOut3; setOut4; setOut5; setOut6; setOut7;
  updateIO();
  setLed;
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  delay(2000);
  //  updateIO();   // Is sigle pin used for LatchIn and LatchOut
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

void TestTouch ( void) {
  cts.setTouchLimit(1);//from 1 to 5
  if (cts.touched()) {
    uint16_t x, y;
    cts.getTSregisters(registers);
    current_touches = cts.getTScoordinates(coordinates, registers);
    if (current_touches < 1) return;

    x = coordinates[0][0];
    y = coordinates[0][1];
    tft.fillCircle(1024 - x, 600 - y, 5, RGB565(0, 255, 0));
  }
}

//-------------------------------------------------------------------------------------
void sendMail(void){

  SerialUSB.println("Invio mail in corso ");

  Serial3.println("AT+IPR=0");
    delay(200);   SerialUSB.println( Serial3.readString()); 
  
  Serial3.println("AT E1");  
    delay(200);   SerialUSB.println( Serial3.readString()); 
  
  Serial3.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
    delay(200);   SerialUSB.println( Serial3.readString()); 
  
  Serial3.println(__APN);
    delay(200);   SerialUSB.println( Serial3.readString()); 
  
  Serial3.println("AT+SAPBR=1,1"); // open GPRS context
    delay(200);   SerialUSB.println( Serial3.readString()); 

  Serial3.println("AT+SAPBR=2,1");
    delay(200);   SerialUSB.println( Serial3.readString()); 
    SerialUSB.println(" If +SAPBR: 1,1, then it is connected to the GPRS");
    
  Serial3.println("AT+EMAILCID=1"); //set email parameters
    delay(200);   SerialUSB.println( Serial3.readString()); 
  
  Serial3.println("AT+EMAILTO=30");
    delay(200);   SerialUSB.println( Serial3.readString()); 

  Serial3.println("AT+SMTPSRV=\"mail.gevaelettronica.it\",25");
    delay(200);   SerialUSB.println( Serial3.readString()); 
  
  Serial3.println("AT+SMTPAUTH=1,\"allarme@gevaelettronica.it\",\"password\"");
    delay(200);   SerialUSB.println( Serial3.readString()); 

  Serial3.println("AT+SMTPFROM=\"allarme@gevaelettronica.it\",\"allarmi\"");
    delay(200);   SerialUSB.println( Serial3.readString());   

  Serial3.println("AT+SMTPRCPT=0,0,\"g.evangelista@gevaelettronica.it\",\"Allarmi\"");
    delay(200);   SerialUSB.println( Serial3.readString()); 

  Serial3.println("AT+SMTPSUB=\"Test Scheda con modem\"");
    delay(200);   SerialUSB.println( Serial3.readString()); 

  Serial3.print("AT+SMTPBODY=");
  Serial3.println(strlen("Test Scheda con modem"));
    delay(200);   SerialUSB.println( Serial3.readString()); 

  Serial3.println("Test Scheda con modem");
  Serial3.println((char)26); 
    delay(200);   SerialUSB.println( Serial3.readString()); 

  Serial3.println("AT+SMTPSEND");
    delay(200);   SerialUSB.println( Serial3.readString()); 

}
