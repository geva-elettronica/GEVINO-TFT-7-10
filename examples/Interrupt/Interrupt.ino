
#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

volatile int CNT = 0;

void setup() {
  SerialUSB.begin(115200);
  
  gevino_io_setup();
  tft.init();
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.setTextColor(WHITE);
  tft.clearScreen(BLACK);

  attachInterrupt(digitalPinToInterrupt(In0_name), interrupt_IN0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(In1_name), interrupt_IN1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(In2_name), interrupt_IN2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(In3_name), interrupt_IN3, CHANGE);
    
}

int OldCNT = 999;
void loop() {

  if(CNT != OldCNT ){
//      tft.setCursor(100, 100);      // Erase the old text.
//      tft.setTextColor(BLACK);      // But here, CNT changing mid-function makes a mess.
//      tft.print(OldCNT);
      tft.fillRect(100, 100, 200, 150, BLACK);      // Erase the old text.
      tft.setCursor(100, 100);
      tft.setTextColor(WHITE);
      tft.print(CNT);  // Write the new value.
      OldCNT = CNT;    
  }

}


void interrupt_IN0() {
  setLed;
  CNT++;
}

void interrupt_IN1() {
  resLed;
  CNT--;
}

void interrupt_IN2() {
  setLed;
  CNT++;
}

void interrupt_IN3() {
  resLed;
  CNT--;
}
