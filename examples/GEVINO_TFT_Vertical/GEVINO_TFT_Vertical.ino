// Example for GEVINO TFT v1.1
// 09/12/2021   Giorgio Evangelista

// Install "Arduino SAMD board"
// Use "Arduino Zero Native Port" or "GEVINO SAM-BA bootloader" 

/*
 * 
 *  
 * 
 * 
 * 
 *  To switch to vertical mode, change the character map written in the flash chip on the PCB.
 *  For more memory for images, it is possible weld  one other chip.
 *  Starting from the folder Arduino\libraries\GEVINI\Vertical
 *  Copy the font.bin file to the SD and put it in the gevino.
 *  Copy font.h and fontExt.h in the library folder Arduino\libraries\GEVINI\
 *  Load this example, or the test, to boot will put several minutes to copy the file, from the SD to the welded flash chip.
 *  
 *  To return to the horizional vision you have to repeat everything from the Arduino\libraries\GEVINI\Horizzontal folder.
 * 
 * 
 * 
 * 
 */





#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

void setup() {

  SerialUSB.begin(115200);

  SerialUSB.println("Begin Test");

  gevino_io_setup();
  tft.init();

// Ruota Font interno.
// Rotate internal font
//  tft.writeReg(RA8876_REG_CCR1, tft.readReg(RA8876_REG_CCR1) | 16 );  // Registro CDh - 
//  tft.writeReg(RA8876_REG_DPCR, tft.readReg(RA8876_REG_DPCR) | 8  );  // registro 12h - VDIR bit
  
  tft.selectInternalFont(RA8876_FONT_SIZE_32);  
  tft.setCursor(0,0);
  CopySDonFlash();          // if exist All_76_Pic_65K.bin or font.bin copy on flash
  tft.setCursor(32,0);
  tft.println("GEVA elettronica");

  delay(500);
  tft.clearScreen(BLACK);
 
}

void loop() {

  int line;

  tft.clearScreen(BLACK);
  line = 0;
    
  tft.setTextColor(WHITE);  // Clear Background

  tft.setFont(&notoSans_8pt);
  tft.setCursor(599, line);
  tft.println("notoSans_8pt");
  line += (8*2);

  tft.setFont(&notoSans_12pt);
  tft.setCursor(599, line);
  tft.println("notoSans_12pt");
  line += (12*2);

  tft.setFont(&notoSans_16pt);
  tft.setCursor(599, line);
  tft.println("notoSans_16pt");
  line += (16*2);

  tft.setFont(&notoSans_20pt);
  tft.setCursor(599, line);
  tft.println("notoSans_20pt");
  line += (20*2);

  tft.setFont(&notoSans_32pt);
  tft.setCursor(599, line);
  tft.println("notoSans_32pt");
  line += (32*2);

  tft.setFont(&notoSans_48pt);
  tft.setCursor(599, line);
  tft.println("notoSans_48pt");
  line += (48*2);

  tft.setFont(&notoSans_64pt);
  tft.setCursor(599, line);
  tft.println("Sans_64pt");
  line += (64*2);

  tft.setFont(&notoSans_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSans_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSans_112pt);
//  tft.setCursor(599, line);
//  tft.println("112pt");

  tft.setFont(&notoSans_48pt);
  tft.setCursor(599, line);
  Colors();

  delay(1500);
  tft.clearScreen(BLACK);
  line = 0;
 
  tft.setTextColor(WHITE);  // Clear Background

  tft.setFont(&notoSansBold_8pt);
  tft.setCursor(599, line);
  tft.println("SansBold_8pt");
  line += (8*2);

  tft.setFont(&notoSansBold_12pt);
  tft.setCursor(599, line);
  tft.println("SansBold_12pt");
  line += (12*2);

  tft.setFont(&notoSansBold_16pt);
  tft.setCursor(599, line);
  tft.println("SansBold_16pt");
  line += (16*2);

  tft.setFont(&notoSansBold_20pt);
  tft.setCursor(599, line);
  tft.println("SansBold_20pt");
  line += (20*2);

  tft.setFont(&notoSansBold_32pt);
  tft.setCursor(599, line);
  tft.println("SansBold_32pt");
  line += (32*2);

  tft.setFont(&notoSansBold_48pt);
  tft.setCursor(599, line);
  tft.println("SansBold_48pt");
  line += (48*2);

  tft.setFont(&notoSansBold_64pt);
  tft.setCursor(599, line);
  tft.println("SansB_64pt");
  line += (64*2);

  tft.setFont(&notoSansBold_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSansBold_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSansBold_112pt);
//  tft.setCursor(599, line);
//  tft.println("B_112pt");

  tft.setFont(&notoSansBold_48pt);
  tft.setCursor(599, line);
  Colors();
  
  delay(1500);
  tft.clearScreen(BLACK);
  line = 0;
  tft.setTextColor(WHITE);  // Clear Background

  tft.setFont(&notoSansItal_8pt);
  tft.setCursor(599, line);
  tft.println("SansItal_8pt");
  line += (8*2);

  tft.setFont(&notoSansItal_12pt);
  tft.setCursor(599, line);
  tft.println("SansItal_12pt");
  line += (12*2);

  tft.setFont(&notoSansItal_16pt);
  tft.setCursor(599, line);
  tft.println("SansItal_16pt");
  line += (16*2);

  tft.setFont(&notoSansItal_20pt);
  tft.setCursor(599, line);
  tft.println("SansItal_20pt");
  line += (20*2);

  tft.setFont(&notoSansItal_32pt);
  tft.setCursor(599, line);
  tft.println("SansItal_32pt");
  line += (32*2);

  tft.setFont(&notoSansItal_48pt);
  tft.setCursor(599, line);
  tft.println("SansItal_48pt");
  line += (48*2);

  tft.setFont(&notoSansItal_64pt);
  tft.setCursor(599, line);
  tft.println("SansI_64pt");
  line += (64*2);

  tft.setFont(&notoSansItal_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSansItal_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSansItal_112pt);
//  tft.setCursor(599, line);
//  tft.println("I_112pt");

  tft.setFont(&notoSansItal_48pt);
  tft.setCursor(599, line);
  Colors();
  
  delay(1500);
  tft.clearScreen(BLACK);
  line = 0;
 
  tft.setTextColor(WHITE);  // Clear Background

  tft.setFont(&notoSerif_8pt);
  tft.setCursor(599, line);
  tft.println("notoSerif_8pt");
  line += (8*2);

  tft.setFont(&notoSerif_12pt);
  tft.setCursor(599, line);
  tft.println("notoSerif_12pt");
  line += (12*2);

  tft.setFont(&notoSerif_16pt);
  tft.setCursor(599, line);
  tft.println("notoSerif_16pt");
  line += (16*2);

  tft.setFont(&notoSerif_20pt);
  tft.setCursor(599, line);
  tft.println("notoSerif_20pt");
  line += (20*2);

  tft.setFont(&notoSerif_32pt);
  tft.setCursor(599, line);
  tft.println("notoSerif_32pt");
  line += (32*2);

  tft.setFont(&notoSerif_48pt);
  tft.setCursor(599, line);
  tft.println("notoSerif_48pt");
  line += (48*2);

  tft.setFont(&notoSerif_64pt);
  tft.setCursor(599, line);
  tft.println("Serif_64pt");
  line += (64*2);

  tft.setFont(&notoSerif_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSerif_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSerif_112pt);
//  tft.setCursor(599, line);
//  tft.println("112pt");

  tft.setFont(&notoSerif_48pt);
  tft.setCursor(599, line);
  Colors();
  
  delay(1500);
  tft.clearScreen(BLACK);
  line = 0;
 
  tft.setTextColor(WHITE);  // Clear Background

  tft.setFont(&notoSerifBold_8pt);
  tft.setCursor(599, line);
  tft.println("SerifBold_8pt");
  line += (8*2);

  tft.setFont(&notoSerifBold_12pt);
  tft.setCursor(599, line);
  tft.println("SerifBold_12pt");
  line += (12*2);

  tft.setFont(&notoSerifBold_16pt);
  tft.setCursor(599, line);
  tft.println("SerifBold_16pt");
  line += (16*2);

  tft.setFont(&notoSerifBold_20pt);
  tft.setCursor(599, line);
  tft.println("SerifBold_20pt");
  line += (20*2);

  tft.setFont(&notoSerifBold_32pt);
  tft.setCursor(599, line);
  tft.println("SerifBold_32pt");
  line += (32*2);

  tft.setFont(&notoSerifBold_48pt);
  tft.setCursor(599, line);
  tft.println("SerifBold_48pt");
  line += (48*2);

  tft.setFont(&notoSerifBold_64pt);
  tft.setCursor(599, line);
  tft.println("SerifB_64pt");
  line += (64*2);

  tft.setFont(&notoSerifBold_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSerifBold_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSerifBold_112pt);
//  tft.setCursor(599, line);
//  tft.println("B_112pt");

  tft.setFont(&notoSerifBold_48pt);
  tft.setCursor(599, line);
  Colors();

  delay(1500);
  tft.clearScreen(BLACK);
  line = 0;  tft.setTextColor(WHITE);  // Clear Background

  tft.setFont(&notoSerifItal_8pt);
  tft.setCursor(599, line);
  tft.println("SerifItal_8pt");
  line += (8*2);

  tft.setFont(&notoSerifItal_12pt);
  tft.setCursor(599, line);
  tft.println("SerifItal_12pt");
  line += (12*2);

  tft.setFont(&notoSerifItal_16pt);
  tft.setCursor(599, line);
  tft.println("SerifItal_16pt");
  line += (16*2);

  tft.setFont(&notoSerifItal_20pt);
  tft.setCursor(599, line);
  tft.println("SerifItal_20pt");
  line += (20*2);

  tft.setFont(&notoSerifItal_32pt);
  tft.setCursor(599, line);
  tft.println("SerifItal_32pt");
  line += (32*2);

  tft.setFont(&notoSerifItal_48pt);
  tft.setCursor(599, line);
  tft.println("SerifItal_48pt");
  line += (48*2);

  tft.setFont(&notoSerifItal_64pt);
  tft.setCursor(599, line);
  tft.println("SerifI_64pt");
  line += (64*2);

  tft.setFont(&notoSerifItal_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSerifItal_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSerifItal_112pt);
//  tft.setCursor(599, line);
//  tft.println("I_112pt");

  tft.setFont(&notoSerifItal_48pt);
  tft.setCursor(599, line);
  Colors();

  delay(1500);
  tft.clearScreen(BLACK);
  line = 0;

  tft.setFont(&notoSansMono_8pt);
  tft.setCursor(599, line);
  tft.println("SansMono_8pt");
  line += (8*2);

  tft.setFont(&notoSansMono_12pt);
  tft.setCursor(599, line);
  tft.println("SansMono_12pt");
  line += (12*2);

  tft.setFont(&notoSansMono_16pt);
  tft.setCursor(599, line);
  tft.println("SansMono_16pt");
  line += (16*2);

  tft.setFont(&notoSansMono_20pt);
  tft.setCursor(599, line);
  tft.println("SansMono_20pt");
  line += (20*2);

  tft.setFont(&notoSansMono_32pt);
  tft.setCursor(599, line);
  tft.println("SansMono_32pt");
  line += (32*2);

  tft.setFont(&notoSansMono_48pt);
  tft.setCursor(599, line);
  tft.println("SansMono_48pt");
  line += (48*2);

  tft.setFont(&notoSansMono_64pt);
  tft.setCursor(599, line);
  tft.println("Mono_64pt");
  line += (64*2);

  tft.setFont(&notoSansMono_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSansMono_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSansMono_112pt);
//  tft.setCursor(599, line);
//  tft.println("112pt");

  tft.setFont(&notoSansMono_48pt);
  tft.setCursor(599, line);
  Colors();

  delay(1500);
  tft.clearScreen(BLACK);
  line = 0;

  tft.setFont(&notoSansMonoBold_8pt);
  tft.setCursor(599, line);
  tft.println("MonoBold_8pt");
  line += (8*2);

  tft.setFont(&notoSansMonoBold_12pt);
  tft.setCursor(599, line);
  tft.println("MonoBold_12pt");
  line += (12*2);

  tft.setFont(&notoSansMonoBold_16pt);
  tft.setCursor(599, line);
  tft.println("MonoBold_16pt");
  line += (16*2);

  tft.setFont(&notoSansMonoBold_20pt);
  tft.setCursor(599, line);
  tft.println("MonoBold_20pt");
  line += (20*2);

  tft.setFont(&notoSansMonoBold_32pt);
  tft.setCursor(599, line);
  tft.println("MonoBold_32pt");
  line += (32*2);

  tft.setFont(&notoSansMonoBold_48pt);
  tft.setCursor(599, line);
  tft.println("MonoBold_48pt");
  line += (48*2);

  tft.setFont(&notoSansMonoBold_64pt);
  tft.setCursor(599, line);
  tft.println("MonoB_64pt");
  line += (64*2);

  tft.setFont(&notoSansMonoBold_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSansMonoBold_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSansMonoBold_112pt);
//  tft.setCursor(599, line);
//  tft.println("B_112pt");

  tft.setFont(&notoSansMonoBold_48pt);
  tft.setCursor(599, line);
  Colors();

  delay(1500);
  tft.clearScreen(BLACK);
  line = 0;

  tft.setFont(&notoSansMonoItal_8pt);
  tft.setCursor(599, line);
  tft.println("MonoItal_8pt");
  line += (8*2);

  tft.setFont(&notoSansMonoItal_12pt);
  tft.setCursor(599, line);
  tft.println("MonoItal_12pt");
  line += (12*2);

  tft.setFont(&notoSansMonoItal_16pt);
  tft.setCursor(599, line);
  tft.println("MonoItal_16pt");
  line += (16*2);

  tft.setFont(&notoSansMonoItal_20pt);
  tft.setCursor(599, line);
  tft.println("MonoItal_20pt");
  line += (20*2);

  tft.setFont(&notoSansMonoItal_32pt);
  tft.setCursor(599, line);
  tft.println("MonoItal_32pt");
  line += (32*2);

  tft.setFont(&notoSansMonoItal_48pt);
  tft.setCursor(599, line);
  tft.println("MonoItal_48pt");
  line += (48*2);

  tft.setFont(&notoSansMonoItal_64pt);
  tft.setCursor(599, line);
  tft.println("MonoI_64pt");
  line += (64*2);

  tft.setFont(&notoSansMonoItal_80pt);
  tft.setCursor(599, line);
  tft.println("80pt");
  line += (80*2);

  tft.setFont(&notoSansMonoItal_96pt);
  tft.setCursor(599, line);
  tft.println("96pt");
  line += (96*2);

//  tft.setFont(&notoSansMonoItal_112pt);
//  tft.setCursor(599, line);
//  tft.println("I_112pt");

  tft.setFont(&notoSansMonoItal_48pt);
  tft.setCursor(599, line);
  Colors();

  delay(1500);

}

void Colors ( void ){
  tft.setTextColor(WHITE);   tft.print("W");
  tft.setTextColor(BLUE);    tft.print("B");
  tft.setTextColor(RED);     tft.print("R");
  tft.setTextColor(GREEN);   tft.print("G");
  tft.setTextColor(CYAN);    tft.print("C");
  tft.setTextColor(MAGENTA); tft.print("M");
  tft.setTextColor(YELLOW);  tft.print("Y");
  tft.setTextColor(BRIGHT_RED);  tft.print("bR");
  tft.setTextColor(LIGHT_GREY);  tft.print("lG");
  tft.setTextColor(DARK_GREY);   tft.print("dG");
  tft.setTextColor(WHITE);  // Clear Background
}
