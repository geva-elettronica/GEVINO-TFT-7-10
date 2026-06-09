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

//Read Status Register (RDSR) Sequence (Command 05)
unsigned char RA8876_SPI_Flash_RDSR(void) {
  unsigned char temp;
  tft.nSS_Active();
  tft.SPI_Master_FIFO_Data_Put(0x05);
  delay(1);
  temp = tft.SPI_Master_FIFO_Data_Put(0xff); //dummy cycle and read back value
  tft.nSS_Inactive();
  return (temp);
}

void WriteFlash(unsigned long flash_addr, char *file, byte erase) {  // erase 0 = No Erase  1 = Erase All  2 = Erase from Pic_Offset to End.
  unsigned long TimeToWrite = millis(),  BlockAddress;
  int wDiv = 0;
  unsigned short i;
  unsigned long k;
  uint8_t buffer[260];
  SdFile ff;
  BlockAddress = flash_addr;
  ff.open(file, FILE_READ);
  //**************************RA8876->Flash
  tft.Enable_SFlash_SPI();
#ifdef WriteC_0
  tft.Select_nSS_drive_on_xnsfcs0();
#else
  tft.Select_nSS_drive_on_xnsfcs1();
#endif
  tft.SPI_Clock_Period(1);
//  tft.Reset_CPOL();  // unsure, check
//  tft.Reset_CPHA();  // unsure, check
#ifdef Flash_256Mb
  tft.nSS_Active();           //nSS port will goes low
  tft.SPI_Master_FIFO_Data_Put(0xB7);   //to enter 4-byte mode: address bit is changed from 24-bit to 32-bit.
  delay(1);
  tft.nSS_Inactive();           //nSS port will goes high
#endif
  tft.print("  Flash Write ");

  if(erase == 1){ 	// Flash Erase All
	do{   // Write Enable
	  tft.nSS_Active();
	  tft.SPI_Master_FIFO_Data_Put(0x06);  //Serial Flash WREN
	  delay(1);
	  tft.nSS_Inactive();   
	  delay(1);
	} while (!(RA8876_SPI_Flash_RDSR() & 0x02));  // WEL = Write Enable Latch

	  tft.nSS_Active();
	  tft.SPI_Master_FIFO_Data_Put(0x60);  //Serial Flash Chip Erase
	  delay(1);
	  tft.nSS_Inactive();
    while(RA8876_SPI_Flash_RDSR() & 0x01);  // Busy = Write in progress

	// Write Disable (WRDI) Sequence (Command 04)
	  tft.nSS_Active();
	  tft.SPI_Master_FIFO_Data_Put(0x04);  //Serial Flash WREN
	  delay(1);
	  tft.nSS_Inactive();
	  while(RA8876_SPI_Flash_RDSR() & 0x01);  // Busy = Write in progress
	  
	  delay(10);
	  tft.print("Eall");
  }

  if(erase == 2){ 	// Flash Erase Pic
  do{
    do{   // Write Enable
      tft.nSS_Active();
      tft.SPI_Master_FIFO_Data_Put(0x06);  //Serial Flash WREN
//      delay(1);
      delayMicroseconds(50);
      tft.nSS_Inactive();   
//    delay(1);
      delayMicroseconds(50);
    } while (!(RA8876_SPI_Flash_RDSR() & 0x02));  // WEL = Write Enable Latch
// 	  tft.println(BlockAddress, HEX);
      tft.nSS_Active();
      tft.SPI_Master_FIFO_Data_Put(0xD8);  //Serial Flash Block 64K Erase
    #ifdef Flash_256Mb
      tft.SPI_Master_FIFO_Data_Put(BlockAddress >> 24);
    #endif
      tft.SPI_Master_FIFO_Data_Put(BlockAddress >> 16);
      tft.SPI_Master_FIFO_Data_Put(BlockAddress >> 8);
      tft.SPI_Master_FIFO_Data_Put(BlockAddress);	  
//      delay(1);
      delayMicroseconds(50);
      tft.nSS_Inactive();
      while(RA8876_SPI_Flash_RDSR() & 0x01);  // Busy = Write in progress
      BlockAddress += 0x10000;
  }while( BlockAddress < 0xFFFFFF ); // Loop to End Flash
	  tft.nSS_Active();
	  tft.SPI_Master_FIFO_Data_Put(0x04);  //Serial Flash WREN
	  delay(1);
	  tft.nSS_Inactive();
	  while(RA8876_SPI_Flash_RDSR() & 0x01);  // Busy = Write in progress
	  delay(10);
	  tft.print("Epic");
  }
//delay(2000);
//return;
  //**************************SD->SAMD->RA8876->FLASH
  do
  {
	ff.read(buffer, 256);

    //Write Enable (WREN) Sequence (Command 06)
    do {
      tft.nSS_Active();
      tft.SPI_Master_FIFO_Data_Put(0x06);  //Serial Flash WREN
//      delay(1);
      delayMicroseconds(50);
      tft.nSS_Inactive();
    } while (!(RA8876_SPI_Flash_RDSR() & 0x02));

//    delay(1);
    delayMicroseconds(50);
    tft.nSS_Active();//nSS port will goes low
    tft.SPI_Master_FIFO_Data_Put(0x02);// set MX25L25635E = PP mode

#ifdef Flash_256Mb
    tft.SPI_Master_FIFO_Data_Put(flash_addr >> 24);
#endif
    tft.SPI_Master_FIFO_Data_Put(flash_addr >> 16);
    tft.SPI_Master_FIFO_Data_Put(flash_addr >> 8);
    tft.SPI_Master_FIFO_Data_Put(flash_addr);


    for (i = 0; i < 256; i++)
    {
      delayMicroseconds(1);
      tft.SPI_Master_FIFO_Data_Put( buffer[i] );
    }

    tft.nSS_Inactive();          //nSS port will goes high

    while (RA8876_SPI_Flash_RDSR() & 0x01);

    flash_addr += 0x0100;
//    delay(1);
    delayMicroseconds(50);
    if ( ++wDiv >= 400) {
      tft.print("w");
      wDiv = 0;
    }
  } while ( ff.available() );

  // Write Disable (WRDI) Sequence (Command 04)
  tft.nSS_Active();
  tft.SPI_Master_FIFO_Data_Put(0x04);  //Serial Flash WREN
  //while( EMTI_Flag()!=1);
  // Clear_EMTI_Flag();
  delay(1);
  tft.nSS_Inactive();
  while (RA8876_SPI_Flash_RDSR() & 0x01);

  RA8876_SPI_Flash_RDSR();

  tft.Check_2D_Busy();

  tft.nSS_Active();         //nSS port will goes low
  tft.SPI_Master_FIFO_Data_Put(0xE9);
  delay(1);
  tft.nSS_Inactive();         //nSS port will goes high

  delay(10);
  tft.Disable_SFlash_SPI();

  ff.close();
  tft.print(" on ");
  tft.print( ( millis() - TimeToWrite ) / 1000 );
  tft.print(" Sec");
}

//#ifdef CopySDonFlash
void CopySDonFlash( void ){
  byte ers = 0;   // Erased
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
      if( ers ) WriteFlash(Pic_Offset, "All_76_Pic_65K.bin", 0);  // Copy, "All_76_Pic_65K.bin" on the chip flash, erase flash chip, pic area
      else  WriteFlash(Pic_Offset, "All_76_Pic_65K.bin", 2);
      gevino_io_setup();    
      tft.init();
      tft.selectInternalFont(RA8876_FONT_SIZE_32);
      tft.println("SD ok, pic copied");
    }   
  }else  tft.println("SD fail");
}
//#endif