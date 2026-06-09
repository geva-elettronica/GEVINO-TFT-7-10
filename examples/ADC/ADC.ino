// Example for GEVINO TFT v1.1
// 30/08/2020   Giorgio Evangelista

// The standard Arduino analogRead() freezes the program for several milliseconds.
// This non-blocking polling method does not.

// Install "Arduino SAMD board"
// Use Arduino Zero Native Port

// install WiFi ESP library github.com/ekstrand/ESP8266wifi
// install eeprom library https://github.com/cmaglie/FlashStorage
// install Adafruit FRAM library https://github.com/adafruit/Adafruit_FRAM_I2C

// The ADC helpers are bundled inside gevino_tft_io.h (no separate ATSAMD21_ADC.h needed).
#include "gevino_tft_io.h"  // Keep this include order
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

void setup() {
  SerialUSB.begin(115200);
  
  // Analog input Setup
  analogPrescaler(ADC_PRESCALER_DIV32);  // DIV4 DIV8 DIV16 DIV32 DIV64 DIV128 DIV256 DIV512 
  analogReadExtended(16); // 16 bit = 256 sample
  analogGain(ADC_GAIN_1);
  analogReference2(ADC_REF_INT1V); // 1.0V reference
  analogCalibrate();
  analogEnable();

  gevino_io_setup();
  tft.init();
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.setTextColor(BLACK);
    
}

void loop() {
  static unsigned long TimerForSec;
  
  analogPolling();  // Read Analog input and put value on analogResult[];

  if( millis() - TimerForSec > 200 ){
    TimerForSec = millis();
    SerialUSB.print(analogResult[0]);
    SerialUSB.print("\t");
    SerialUSB.print(analogResult[1]);
    SerialUSB.print("\t");
    SerialUSB.print(analogResult[2]);
    SerialUSB.print("\t");
    SerialUSB.print(analogResult[3]);
    SerialUSB.print("\t");
    SerialUSB.println(analogResult[4]);

    tft.clearScreen(WHITE);    
    tft.println(analogResult[0]);
    tft.println(analogResult[1]);
    tft.println(analogResult[2]);
    tft.println(analogResult[3]);
    tft.println(analogResult[4]);
}


}
