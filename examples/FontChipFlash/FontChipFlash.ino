#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

void setup() {
  gevino_io_setup();
  tft.init();
  
//  SerialUSB.begin(115200);
//  while(!SerialUSB);

  tft.setFont(&notoSans_8pt);
  tft.setCursor(0, 0);

  CopySDonFlash();          // if exist pic.bin or font.bin copy on flash

  // ----------------------  Try internal font
  tft.setTextScale(1);
  tft.setTextColor(RED);
  tft.setCursor(350,200);
  tft.selectInternalFont(RA8876_FONT_SIZE_16);
  tft.println("Internal 16");
  tft.setTextColor(BLUE);
  tft.setCursor(350,225);
  tft.selectInternalFont(RA8876_FONT_SIZE_24);
  tft.println("Internal 24");
  tft.setTextColor(GREEN);
  tft.setCursor(350,250);
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.println("Internal 32");
  
  tft.setFont(&notoSansBold_112pt);
  tft.setTextColor(WHITE, 0);
  tft.setCursor(0, 0);
  tft.println("Geva");

  tft.setFont(&notoSansBold_48pt);
  tft.setTextColor(RED, YELLOW);  // Color Background
  tft.setCursor(100, 20);
  tft.println(" Geva g ");

  tft.setFont(&notoSansBold_48pt);
  tft.setTextColor(BLUE);  // Clear Background
  tft.setCursor(100, 70);
  tft.println(" Geva g ");

  tft.setCursor(0, 150);
  tft.setTextColor(WHITE);

  tft.setFont(&notoSans_32pt);
  tft.println("noto Sans");
  tft.setFont(&notoSansBold_32pt);
  tft.println("noto Sans Bold");
  tft.setFont(&notoSansItal_32pt);
  tft.println("noto Sans Ital");
  tft.setFont(&notoSansMono_32pt);
  tft.println("noto Sans Mono");
  tft.setFont(&notoSansMonoBold_32pt);
  tft.println("noto Sans Mono Bold");
  tft.setFont(&notoSerif_32pt);
  tft.println("noto Serif");
  tft.setFont(&notoSerifBold_32pt);
  tft.println("noto Serif Bold");

  tft.setFont(&notoSans_8pt);
  tft.setCursor(600, 0);
  tft.println("8 pt");
  tft.setFont(&notoSans_12pt);
  tft.setCursor(600, 12);
  tft.println("12 pt");
  tft.setFont(&notoSans_20pt);
  tft.setCursor(600, 32);
  tft.println("20 pt");
  tft.setFont(&notoSans_32pt);
  tft.setCursor(600, 60);
  tft.println("32 pt");
  tft.setFont(&notoSans_48pt);
  tft.setCursor(600, 102);
  tft.println("48 pt");
  tft.setFont(&notoSans_64pt);
  tft.setCursor(600, 160);
  tft.println("64 pt");
  tft.setFont(&notoSans_80pt);
  tft.setCursor(600, 234);
  tft.println("80 pt");
  tft.setFont(&notoSans_96pt);
  tft.setCursor(600, 320);
  tft.println("96 pt");
  tft.setFont(&notoSans_112pt);
  tft.setCursor(600, 440);
  tft.println("112 pt");

  uint16_t col = 0x000000;
  tft.setFont(&notoSansBold_80pt);
  for(int f = 0; f < 0xFF; f++){
    col += 0x0001;
    tft.setTextColor(col);  // Color Background
    tft.setCursor(50, 480);
    tft.println("Geva");
    delay(5);
  }

  tft.setFont(&notoSans_112pt);
}


void loop(){
  tft.setCursor(rand() % 900, rand() % 550);
  tft.setTextColor(rand() % 0xFFFF);
  tft.println("Geva");
}
//  tft.setFont(notoSans_8pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSans_12pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSans_20pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSans_32pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSans_48pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSans_64pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSans_80pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSans_96pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSans_112pt);
//  pr(0xFFFFFF);
//
//  tft.setFont(notoSansBold_8pt);
//  pr(0x0000FF);
//  tft.setFont(notoSansBold_12pt);
//  pr(0x0000FF);
//  tft.setFont(notoSansBold_20pt);
//  pr(0x0000FF);
//  tft.setFont(notoSansBold_32pt);
//  pr(0x0000FF);
//  tft.setFont(notoSansBold_48pt);
//  pr(0x0000FF);
//  tft.setFont(notoSansBold_64pt);
//  pr(0x0000FF);
//  tft.setFont(notoSansBold_80pt);
//  pr(0x0000FF);
//  tft.setFont(notoSansBold_96pt);
//  pr(0x0000FF);
//  tft.setFont(notoSansBold_112pt);
//  pr(0x0000FF);
//
//  tft.setFont(notoSansItal_8pt);
//  pr(0x00FF00);
//  tft.SetFont(notoSansItal_12pt);
//  pr(0x00FF00);
//  tft.setFont(notoSansItal_20pt);
//  pr(0x00FF00);
//  tft.SetFont(notoSansItal_32pt);
//  pr(0x00FF00);
//  tft.setFont(notoSansItal_48pt);
//  pr(0x00FF00);
//  tft.setFont(notoSansItal_64pt);
//  pr(0x00FF00);
//  tft.setFont(notoSansItal_80pt);
//  pr(0x00FF00);
//  tft.setFont(notoSansItal_96pt);
//  pr(0x00FF00);
//  tft.setFont(notoSansItal_112pt);
//  pr(0x00FF00);
//  
//  tft.setFont(notoSansMono_8pt);
//  pr(0xFF0000);
//  tft.setFont(notoSansMono_12pt);
//  pr(0xFF0000);
//  tft.setFont(notoSansMono_20pt);
//  pr(0xFF0000);
//  tft.setFont(notoSansMono_32pt);
//  pr(0xFF0000);
//  tft.setFont(notoSansMono_48pt);
//  pr(0xFF0000);
//  tft.setFont(notoSansMono_64pt);
//  pr(0xFF0000);
//  tft.setFont(notoSansMono_80pt);
//  pr(0xFF0000);
//  tft.setFont(notoSansMono_96pt);
//  pr(0xFF0000);
//  tft.setFont(notoSansMono_112pt);
//  pr(0xFF0000);
//
//  tft.setFont(notoSansMonoBold_8pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSansMonoBold_12pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSansMonoBold_20pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSansMonoBold_32pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSansMonoBold_48pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSansMonoBold_64pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSansMonoBold_80pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSansMonoBold_96pt);
//  pr(0xFFFFFF);
//  tft.setFont(notoSansMonoBold_112pt);
//  pr(0xFFFFFF);
//
//
//
////  tft.setCursor(100, 100);
////  tft.println("Giorgio");
//  }
//
//
//void pr(long col){
//  tft.setTextColor(col, 0x000050);
//  tft.clearScreen(0);  
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col, 0x000050);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col, 0x000050);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col, 0x000050);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col, 0x000050);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col, 0x000050);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
//  tft.setTextColor(col);
//  tft.println("012345 abcdefgh ABCDEFGHIJ !\"#$%&'()*+,-./:;<=>?@[\\]^_`");
////  tft.println("GEVA elett");
////  tft.println("GEVINO TFT");
////  tft.println("ARM SAMD");
////  tft.println("Optoisolated");
//  delay(1000);  
//}
