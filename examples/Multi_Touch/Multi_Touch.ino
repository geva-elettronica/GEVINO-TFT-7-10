/*
 * GEVINO TFT
 * Multi Touch Test
 * 
 */

#include <stdint.h>
#include <Wire.h>
#include <SPI.h>
#include "Arduino.h"
#include "Print.h"
#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

uint8_t addr  = 0x38;  //CTP IIC ADDRESS

#define FT5316_INT   38  


 char stringEnd ='\0';
 char string1[] = {0xa6,0xb0,0xa4,0xe9,0xaa,0x46,0xa4,0xe8,stringEnd};  //BIG5
 char string2[] = {0xbb,0xb6,0xd3,0xad,0xca,0xb9,0xd3,0xc3,stringEnd};  //BG2312

enum {
  eNORMAL = 0,
  eTEST   = 0x04,
  eSYSTEM = 0x01
};

struct TouchLocation
{
  uint16_t x;
  uint16_t y;
};

TouchLocation touchLocations[5];

uint8_t readFT5316TouchRegister( uint8_t reg );
uint8_t readFT5316TouchLocation( TouchLocation * pLoc, uint8_t num );
uint8_t readFT5316TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len );

uint32_t dist(const TouchLocation & loc);
uint32_t dist(const TouchLocation & loc1, const TouchLocation & loc2);

bool sameLoc( const TouchLocation & loc, const TouchLocation & loc2 );


uint8_t buf[30];

uint8_t readFT5316TouchRegister( uint8_t reg )
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  uint8_t returned = Wire.requestFrom(addr, uint8_t(1) );    // request 6 uint8_ts from slave device #2
  
  if (Wire.available())
  {
    retVal = Wire.read();
  }
  
  return retVal;
}

uint8_t readFT5316TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len )
{
  Wire.beginTransmission(addr);
  Wire.write( regAddr );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  uint8_t returned = Wire.requestFrom(addr, len);    // request 1 bytes from slave device #2
  
  uint8_t i;
  for (i = 0; (i < len) && Wire.available(); i++)
  {
    pBuf[i] = Wire.read();
  }
  
  return i;
}

uint8_t readFT5316TouchLocation( TouchLocation * pLoc, uint8_t num )
{
  uint8_t retVal = 0;
  uint8_t i;
  uint8_t k;
  
  do
  {
    if (!pLoc) break; // must have a buffer
    if (!num)  break; // must be able to take at least one
    
    uint8_t status = readFT5316TouchRegister(2);
    
    static uint8_t tbuf[40];
    
    if ((status & 0x0f) == 0) break; // no points detected
    
    uint8_t hitPoints = status & 0x0f;
    
    SerialUSB.print("number of hit points = ");
    SerialUSB.println( hitPoints );
    
    readFT5316TouchAddr( 0x03, tbuf, hitPoints*6);
    
    for (k=0,i = 0; (i < hitPoints*6)&&(k < num); k++, i += 6)
    {
      pLoc[k].x = (tbuf[i+0] & 0x0f) << 8 | tbuf[i+1];
      pLoc[k].y = (tbuf[i+2] & 0x0f) << 8 | tbuf[i+3];
    }
    
    retVal = k;
    
  } while (0);
  
  return retVal;
}

void writeFT5316TouchRegister( uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  Wire.write( val );  // value
  
  uint8_t retVal = Wire.endTransmission();  
}

void readOriginValues(void)
{
  writeFT5316TouchRegister(0, eTEST);
  delay(1);
  //uint8_t originLength = readFT5316TouchAddr(0x08, buf, 8 );
  uint8_t originXH = readFT5316TouchRegister(0x08);
  uint8_t originXL = readFT5316TouchRegister(0x09);
  uint8_t originYH = readFT5316TouchRegister(0x0a);
  uint8_t originYL = readFT5316TouchRegister(0x0b);
  
  uint8_t widthXH  = readFT5316TouchRegister(0x0c);
  uint8_t widthXL  = readFT5316TouchRegister(0x0d);
  uint8_t widthYH  = readFT5316TouchRegister(0x0e);
  uint8_t widthYL  = readFT5316TouchRegister(0x0f);
  
}

 
void setup() {
  SerialUSB.begin(115200);
//  while(!SerialUSB);   // Wait for the USB serial from the PC to open
  gevino_io_setup();
  tft.init();
  SerialUSB.println("RA8876 Lite");
  Wire.begin();        // join i2c bus (address optional for master)   
 
  readOriginValues();
  writeFT5316TouchRegister(0, eNORMAL); // device mode = Normal  
  uint8_t periodMonitor = readFT5316TouchRegister(0x89);  
  SerialUSB.print("periodMonitor = ");
  SerialUSB.println( periodMonitor, HEX );
  
  uint8_t  lenLibVersion = readFT5316TouchAddr(0x0a1, buf, 2 );
  if (lenLibVersion)
  {
    uint16_t libVersion = (buf[0] << 8) | buf[1];
  
    SerialUSB.print("lib version = ");
    SerialUSB.println( libVersion, HEX);
  }
  else
  {
    SerialUSB.println("lib version length is zero");
  }
 
  uint8_t firmwareId = readFT5316TouchRegister( 0xa6 );
  
  SerialUSB.print("firmware ID = ");
  SerialUSB.println( firmwareId); 
   
  pinMode     (FT5316_INT, INPUT);
  //digitalWrite(FT5316_INT, HIGH );
  
}

uint32_t dist(const TouchLocation & loc)
{
  uint32_t retVal = 0;
  
  uint32_t x = loc.x;
  uint32_t y = loc.y;
  
  retVal = x*x + y*y;
  
  return retVal;
}

uint32_t dist(const TouchLocation & loc1, const TouchLocation & loc2)
{
  uint32_t retVal = 0;
  
  uint32_t x = loc1.x - loc2.x;
  uint32_t y = loc1.y - loc2.y;
  
  retVal = sqrt(x*x + y*y);
  
  return retVal;
}

bool sameLoc( const TouchLocation & loc, const TouchLocation & loc2 )
{
  return dist(loc,loc2) < 50;
}


void loop() {

  static uint16_t w = 1024;
  static uint16_t h = 600;  

  uint8_t attention = digitalRead(FT5316_INT);
  uint8_t flag=1;
   unsigned int i;
   double float_data;   

 
 while(flag) 
  { attention = digitalRead(FT5316_INT);
     if (!attention) 
    {  
      SerialUSB.println("Touch: ");
      
      uint8_t count = readFT5316TouchLocation( touchLocations, 5 );
  
      for (i = 0; i < count; i++)
      {
//            if ((touchLocations[i].x>960) &&(touchLocations[i].y<20)) flag=0;
//  
//            if ((touchLocations[i].x<64) &&(touchLocations[i].y<20))
//              {  tft.canvasImageStartAddress(PAGE1_START_ADDR);
//                tft.canvasImageWidth(SCREEN_WIDTH);
//                tft.activeWindowXY(0,0);
//                tft.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
//                tft.drawSquareFill(0, 0, 1023, 599, COLOR65K_BLACK);
//                tft.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM,RA8876_CHAR_HEIGHT_16,RA8876_SELECT_8859_1);//cch
//                tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE,RA8876_TEXT_WIDTH_ENLARGEMENT_X1,RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
//                tft.textColor(COLOR65K_WHITE,COLOR65K_BLACK);
//                tft.putString(10,0," www.buydisplay.com  Capacitive touch screen test.Please touch the screen!"); 
//               tft.textColor(COLOR65K_BLACK,COLOR65K_WHITE);  
//               tft.putString(960,580," clear"); 
//                tft.putString(0,580,"  Exit");
//              }                         
//          
//           tft.setTextParameter1(RA8876_SELECT_INTERNAL_CGROM,RA8876_CHAR_HEIGHT_16,RA8876_SELECT_8859_1);//cch
//            tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE,RA8876_TEXT_WIDTH_ENLARGEMENT_X1,RA8876_TEXT_HEIGHT_ENLARGEMENT_X1); 
//        tft.textColor(COLOR65K_WHITE,COLOR65K_BLACK);
//          snprintf((char*)buf,sizeof(buf),"(%3d,%3d)",touchLocations[i].x,touchLocations[i].y);
//
//        const  char *str=(const char *)buf;
//         tft.textMode(true);
//        tft.setTextCursor(380,380+16*i);
//        tft.ramAccessPrepare();
//        while(*str != '\0')
//        {
//        tft.checkWriteFifoNotFull();  
//        tft.lcdDataWrite(*str);
//        ++str; 
//        } 
//        tft.check2dBusy();
//        tft.textMode(false);
       
               if(i==0)  tft.fillCircle(w-touchLocations[i].x,h-touchLocations[i].y, 3, RED);  
          else if(i==1)  tft.fillCircle(w-touchLocations[i].x,h-touchLocations[i].y, 3, GREEN); 
          else if(i==2)  tft.fillCircle(w-touchLocations[i].x,h-touchLocations[i].y, 3, BLUE);        
          else if(i==3)  tft.fillCircle(w-touchLocations[i].x,h-touchLocations[i].y, 3, CYAN); 
          else if(i==4)  tft.fillCircle(w-touchLocations[i].x,h-touchLocations[i].y, 3, MAGENTA); 
        }
     } 
  }
}
