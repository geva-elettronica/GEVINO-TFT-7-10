// Example for GEVINO TFT v1.1
// 29/01/2021   Giorgio Evangelista

// Install "Arduino SAMD board"
// Use Arduino Zero Native Port

#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"

void setup() 
{

  gevino_io_setup();
  
  // Analog Out on Pin A0
  //  pinMode(A0, OUTPUT);       // Not declare that it will not work
  DAC->CTRLB.bit.REFSEL = 0;  // Set Up 1V as DAC Output reference.
  analogWrite(A0, 0);         // Initialize Dac  to Zero
}


void loop() 
{
  analogWrite(A0, 1023);         // Set DAC to Max
  delay(3000);
  analogWrite(A0, 512);         // Set DAC to Max
  delay(3000);
  analogWrite(A0, 0);         // Set DAC to Max
  delay(3000);
}
