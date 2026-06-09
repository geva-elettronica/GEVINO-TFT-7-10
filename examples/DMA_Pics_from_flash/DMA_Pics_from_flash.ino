// Example for GEVINO TFT v1.1
// 08/09/2020   Giorgio Evangelista

#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

void setup() {
  SerialUSB.begin(115200);

  gevino_io_setup();
  tft.init();
  tft.selectInternalFont(RA8876_FONT_SIZE_32);  
  CopySDonFlash();          // if exist All_76_Pic_65K.bin or font.bin copy on flash

  tft.clearScreen(WHITE);
  tft.setFont(&notoSansBold_64pt);
  tft.setTextColor(BLUE);  // Clear Background

  tft.setCursor(700,500);
  tft.println("GEVA");

  tft.pic(button,0,0);
  delay(1500);

}

void loop() {
  static int dly = 1000;

  pinMode(5, OUTPUT);

  digitalWrite( 5, HIGH);
  tft.pic(art,0,0);
  digitalWrite( 5, LOW);
  delay(dly);
  tft.pic(scada,0,0);
  delay(dly);
  tft.pic(gauge,0,0);
  delay(dly);
  tft.pic(box,0,0);
  delay(dly);

  if(dly) dly -= 500;
}
