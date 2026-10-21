// TFT
#include "gevinoTFT_RA8876.h"
#define RA8876_CS        42
#define RA8876_RESET     -1
RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);
//#ifdef CopySDonFlash
#include "font.h"
//#endif

  // --------------------------------------  Program chip Flash ----------------------------------
//#define WriteC_0 // Otherwise Can1
//#ifdef Flash_256Mb    // 32 bit
#undef code
#include <SdFat.h>
SdFat sd;

// These two used to carry their own copy of the flash writer. They now call
// the one in the library (RA8876::writeFlash), which shifts the data into the
// controller in blocks instead of one chip-select frame per byte and erases
// only the 64 KB blocks it writes: minutes less on a full image.
//
// 'erase' keeps its meaning for existing sketches: 0 writes on what is there,
// 1 and 2 both erase each block just before writing into it. Nothing erases
// the whole chip any more, so a call with erase = 0 must be aimed at an area
// that is already blank.
unsigned char RA8876_SPI_Flash_RDSR(void) {     // Read Status Register (05h)
  return tft.flashReadStatus();
}

void WriteFlash(unsigned long flash_addr, char *file, byte erase) {
  tft.writeFlash(flash_addr, file, erase);
}

//#ifdef CopySDonFlash
void CopySDonFlash( void ){
  byte ers = 0;   // fonts written: the canvas has to be reloaded
  if( sd.begin(12, SD_SCK_MHZ(12) ) ){
    tft.println("SD ok");
    if (sd.exists("font.bin")){
      WriteFlash(0, "font.bin", 1);  // Copy, From Address 0, "font.bin" on the chip flash, before erase flash chip
      ers = 1;
      gevino_io_setup();    
      tft.init();
      tft.selectInternalFont(RA8876_FONT_SIZE_32);
      tft.println("SD ok, font copied");
    }
    if (sd.exists("All_76_Pic_65K.bin")){
      WriteFlash(Pic_Offset, "All_76_Pic_65K.bin", 2);  // erases the blocks it writes, whatever the font did
      gevino_io_setup();    
      tft.init();
      tft.selectInternalFont(RA8876_FONT_SIZE_32);
      tft.println("SD ok, pic copied");
    }   
  }else  tft.println("SD fail");
}
//#endif