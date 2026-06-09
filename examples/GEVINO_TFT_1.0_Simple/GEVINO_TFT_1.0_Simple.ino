// Example for GEVINO TFT v1.1
// 18/12/2019   Giorgio Evangelista

// Install "Arduino SAMD board"
// Use Arduino Zero Native Port

#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

// Touch
#include <FT5206.h>
#define CTP_INT          38    // touch data ready for read from FT5206 touch controller
FT5206 cts = FT5206(CTP_INT);
uint8_t registers[FT5206_REGISTERS];
uint16_t coordinates[5][2];
uint8_t current_touches = 0;

String str;

void setup() {

  SerialUSB.begin(115200);
  Serial.begin(115200);     // ESP Wi-Fi
  Serial1.begin(115200);    // RS485
  Serial2.begin(38400);     // BT HC-06-07-08  + RS232
  Serial3.begin(57600);     // GSM SIM800L

  gevino_io_setup();
  tft.init();
  tft.selectInternalFont(RA8876_FONT_SIZE_32);  

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
