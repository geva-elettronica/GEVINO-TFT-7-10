// Example for GEVINO TFT v1.1
// 18/12/2019   Giorgio Evangelista

// Install "Arduino SAMD board"
// Use Arduino Zero Native Port

#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

// Touch: 7" (FT5206) and 10" (GT9271) are both handled by the library,
// which auto-detects the panel fitted to the board.
#include "gevino_tft_touch.h"

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

  gevino_touch_setup();   // probes 0x38 / 0x5D: returns 7, 10 or 0
  SerialUSB.print("Touch panel: ");
  SerialUSB.println(gevino_touch_name());
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
  int16_t x, y;
  // The library returns screen coordinates (0..1023, 0..599) for both panels.
  if (gevino_touch_read(&x, &y)) {
    tft.fillCircle(x, y, 5, RGB565(0, 255, 0));
  }
}
