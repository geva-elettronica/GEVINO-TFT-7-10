#ifndef _GEVINO_TFT_IO_H_
#define _GEVINO_TFT_IO_H_

/*
  GEVINO TFT - board I/O support header
  GEVA Elettronica - https://www.gevaelettronica.it

  HMI PLC with a 7" or 10" capacitive-touch TFT (1024x600, Raio RA8876
  controller) based on the Microchip SAMD21 (ARM Cortex-M0+), Arduino Zero
  compatible. The 7" and 10" models share the SAME board - only the glass
  (TFT panel + capacitive touch controller) changes:
    - 7"  : FT5206 touch controller (I2C 0x38)
    - 10" : GT9271 touch controller (I2C 0x5D)

  This header bundles:
    1. Low-level SAMD21 ADC helpers        (originally ATSAMD21_ADC.h, Blake Felt)
    2. Serial2 / Serial3 (extra SERCOM UARTs) and their ISRs
    3. Pin definitions and I/O macros       (digital I/O via 74HC595/74HC165)
    4. Board setup and shift-register I/O update
    5. Non-blocking analog polling
    6. A lightweight Timer class            (GEVA Elettronica)

  The RA8876 display driver lives in separate files (gevinoTFT_RA8876.h/.cpp);
  this header only covers the board's digital/analog I/O, serial ports and ADC.

  Usage (respect this include order):
    #include "gevino_tft_io.h"
    #include "All_76_Pic_65K.h"   // image data (defines, uses the `code`/PROGMEM macro)
    #include "gevino_tft_display.h"

  IMPORTANT:
    - This header instantiates the Serial2/Serial3 SERCOM UART objects and their
      interrupt handlers and defines global registers (OutReg/InReg/analogResult),
      so include it in exactly ONE translation unit (your main .ino).
    - Call gevino_io_setup() in setup(), AFTER Serial2.begin()/Serial3.begin().
    - Do NOT initialise the I/O yourself: gevino_io_setup() does it for you.

  Changelog
    22/11/2022   More output current on some pins (DRVSTR).
    08/06/2026   Bundled ATSAMD21_ADC.h and a Timer class into this header.
*/

#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>        // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
#define pic_def
#define code PROGMEM

// =============================================================================
// 1. Low-level SAMD21 ADC helpers
//    Originally ATSAMD21_ADC.h by Blake Felt. Generic SAMD21 ADC utilities,
//    marked `inline` so the header stays safe to include.
// =============================================================================

#ifndef ADC_CTRLB_RESSEL_12BIT_Val
#define ADC_CTRLB_RESSEL_8BIT_Val   0x03
#define ADC_CTRLB_RESSEL_10BIT_Val  0x02 // default by Arduino
#define ADC_CTRLB_RESSEL_12BIT_Val  0x00
#endif
#define ADC_CTRLB_RESSEL_16BIT_Val  0x01 // used for averaging mode output

#define ADC_PIN_TEMP                0x18 // positive mux, pg 870
#define ADC_PIN_BANDGAP             0x19
#define ADC_PIN_SCALEDCOREVCC       0x1A
#define ADC_PIN_SCALEDIOVCC         0x1B
#define ADC_PIN_DAC                 0x1C

#define ADC_PIN_GND                 0x18 // negative mux, pg 869
#define ADC_PIN_IOGND               0x19

#define ADC_GAIN_1                  0x00 // pg 868
#define ADC_GAIN_2                  0x01
#define ADC_GAIN_4                  0x02
#define ADC_GAIN_8                  0x03
#define ADC_GAIN_16                 0x04
#define ADC_GAIN1_DIV2              0x0F // default by Arduino

#define ADC_REF_INT1V               0x00 // 1.0V reference, pg 861
#define ADC_REF_INTVCC0             0x01 // 1/1.48 VDDANA
#define ADC_REF_INTVCC1             0x02 // 1/2 VDDANA (only for VDDANA > 2.0V) // default
#define ADC_REF_VREFA               0x03 // external reference
#define ADC_REF_VREFB               0x04 // external reference

#define ADC_PRESCALER_DIV4          0x00 // pg 864
#define ADC_PRESCALER_DIV8          0x01
#define ADC_PRESCALER_DIV16         0x02
#define ADC_PRESCALER_DIV32         0x03
#define ADC_PRESCALER_DIV64         0x04
#define ADC_PRESCALER_DIV128        0x05
#define ADC_PRESCALER_DIV256        0x06
#define ADC_PRESCALER_DIV512        0x07 // Arduino default

// NVM Software Calibration Area Mapping, pg 32. Address starting at NVMCTRL_OTP4.
#define NVM_ADC_LINEARITY_POS         27
#define NVM_ADC_LINEARITY_SIZE         8
#define NVM_ADC_BIASCAL_POS           35
#define NVM_ADC_BIASCAL_SIZE           3

// Wait for synchronization of registers between the clock domains
static __inline__ void syncADC() __attribute__((always_inline, unused));
static void syncADC() {
  while(ADC->STATUS.bit.SYNCBUSY == 1);
}
static __inline__ void syncDAC() __attribute__((always_inline, unused));
static void syncDAC() {
  while (DAC->STATUS.bit.SYNCBUSY == 1);
}

// Allows the ADC to read 8/10/12 bits normally or 13..16 bits via oversampling.
inline uint8_t analogReadExtended(uint8_t bits) {
  switch(bits) {
    case 8:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_8BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x0;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x0;
      return 0;
    case 10:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_10BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x0;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x0;
      return 0;
    case 12:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_12BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x0;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x0;
      return 0;
    case 13:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x1;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x2;
      return 0;
    case 14:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x2;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x4;
      return 0;
    case 15:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x1;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x6;
      return 0;
    case 16:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x0;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x8;
      return 0;
    default:
      return -1;
  }
}

// returns the internal ADC channel of the specified Arduino pin
inline uint8_t internalPinValue(uint8_t pin) {
  return g_APinDescription[pin].ulADCChannelNumber;
}

// reads the difference between two pins (single conversion, blocking)
inline int16_t analogDifferential(uint8_t pin_pos, uint8_t pin_neg) {
  if(pin_pos<A0) pin_pos += A0;
  if(pin_neg<A0) pin_neg += A0;

  if((g_APinDescription[pin_neg].ulADCChannelNumber>0x07) && (pin_neg<ADC_PIN_GND)) {
    return 0;
  }

  uint32_t value_read = 0;

  pinPeripheral(pin_pos, PIO_ANALOG);
  pinPeripheral(pin_neg, PIO_ANALOG);

  if((pin_pos == A0) || (pin_neg == A0)) { // Disable DAC
    syncDAC();
    DAC->CTRLA.bit.ENABLE = 0x00;
    syncDAC();
  }

  syncADC();
  ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[pin_pos].ulADCChannelNumber;
  ADC->INPUTCTRL.bit.MUXNEG = g_APinDescription[pin_neg].ulADCChannelNumber;

  syncADC();
  ADC->CTRLA.bit.ENABLE = 0x01;
  ADC->CTRLB.bit.DIFFMODE = 1;

  syncADC();
  ADC->SWTRIG.bit.START = 1;

  ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
  syncADC();

  ADC->SWTRIG.bit.START = 1; // restart, as changing inputs messes up first conversion

  while(ADC->INTFLAG.bit.RESRDY == 0);
  value_read = ADC->RESULT.reg;

  syncADC();
  ADC->CTRLA.bit.ENABLE = 0x00;
  ADC->CTRLB.bit.DIFFMODE = 0;
  ADC->INPUTCTRL.bit.MUXNEG = ADC_PIN_GND;
  syncADC();

  return value_read;
}

// raw differential read: no error checking, no pin reconfiguration, no DAC handling
inline int16_t analogDifferentialRaw(uint8_t mux_pos, uint8_t mux_neg) {
  uint32_t value_read = 0;

  syncADC();
  ADC->INPUTCTRL.bit.MUXPOS = mux_pos;
  ADC->INPUTCTRL.bit.MUXNEG = mux_neg;

  syncADC();
  ADC->CTRLA.bit.ENABLE = 0x01;
  ADC->CTRLB.bit.DIFFMODE = 1;

  syncADC();
  ADC->SWTRIG.bit.START = 1;

  ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
  syncADC();

  ADC->SWTRIG.bit.START = 1;

  while(ADC->INTFLAG.bit.RESRDY == 0);
  value_read = ADC->RESULT.reg;

  syncADC();
  ADC->CTRLA.bit.ENABLE = 0x00;
  ADC->CTRLB.bit.DIFFMODE = 0;
  ADC->INPUTCTRL.bit.MUXNEG = ADC_PIN_GND;
  syncADC();

  return value_read;
}

// sets the gain of the ADC (pg 868)
inline void analogGain(uint8_t gain) {
  syncADC();
  ADC->INPUTCTRL.bit.GAIN = gain;
  syncADC();
}

// calibrates bias and linearity from the NVM software calibration area (pg 32, 882)
inline void analogCalibrate() {
  syncADC();
  uint32_t adc_linearity = (*((uint32_t *)(NVMCTRL_OTP4)
          + (NVM_ADC_LINEARITY_POS / 32))
          >> (NVM_ADC_LINEARITY_POS % 32))
          & ((1 << NVM_ADC_LINEARITY_SIZE) - 1);

  uint32_t adc_biascal = (*((uint32_t *)(NVMCTRL_OTP4)
          + (NVM_ADC_BIASCAL_POS / 32))
          >> (NVM_ADC_BIASCAL_POS % 32))
          & ((1 << NVM_ADC_BIASCAL_SIZE) - 1);

  ADC->CALIB.bit.LINEARITY_CAL = adc_linearity;
  ADC->CALIB.bit.BIAS_CAL = adc_biascal;
  syncADC();
}

// sets the analog reference voltage with all available options (pg 861)
inline void analogReference2(uint8_t ref) {
  syncADC();
  ADC->REFCTRL.bit.REFSEL = ref;
  syncADC();
}

// enables reference buffer offset compensation (pg 861)
inline void analogReferenceCompensation(uint8_t val) {
  if(val>0) val = 1;
  syncADC();
  ADC->REFCTRL.bit.REFCOMP = val;
  syncADC();
}

// sets the ADC clock prescaler relative to the peripheral clock (pg 864)
inline void analogPrescaler(uint8_t val) {
  syncADC();
  ADC->CTRLB.bit.PRESCALER = val;
  syncADC();
}

// resets the ADC (pg 860)
inline void analogReset() {
  syncADC();
  ADC->CTRLA.bit.SWRST = 1;
  while(ADC->CTRLA.bit.SWRST==1);
  syncADC();
}

// =============================================================================
// 2. Extra SERCOM UARTs: Serial2 and Serial3
//    These instantiate global objects and ISRs -> include this header once.
// =============================================================================
Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM1_Handler()  {   Serial2.IrqHandler(); }
Uart Serial3 (&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM2_Handler()  {   Serial3.IrqHandler(); }

// =============================================================================
// 3. Pin definitions and I/O macros (GEVA Elettronica)
//    12 opto-isolated inputs and 8 opto-isolated outputs. The 8 outputs and
//    8 of the inputs go through a 74HC595 (out) / 74HC165 (in) shift-register
//    chain driven over SPI (see updateIO()); inputs In0..In3 are direct GPIO,
//    4 of which are high-speed / interrupt capable.
// =============================================================================
byte OutReg = 0, InReg = 0;

//#define TestSDpin

// Bit-Field helpers
#define SetBit(x,y) ((x) |= (1<<y))
#define ClearBit(x,y) ((x) &=(~(1<<y)))
#define CheckBit(x,y) ((x) & (1<<y))
#define SetBitInArray(Dest, bit)  ((*(Dest+(bit/8))) |= (1<<(bit%8)))
#define ClearBitInArray(Dest, bit)  ((*(Dest+(bit/8))) &= (~(1<<(bit%8))))
#define CheckBitInArray(Dest, bit)  ((*(Dest+(bit/8))) &  (1<<bit%8))

#define latchPinO 6      //Pin connected to ST_CP of 74HC595 Output
#define latchPinI 2      //Pin connected to ST_CP of 74HC165 Input
#define ETH_CS 5
#define TFT_CS 42
#define SD_CS 12
#define CH376_CS 45
#define CH376_INT 27
#define Rs485_TxEn  7
#define In0_name  18  // A4
#define In1_name  19  // A5
#define In2_name  8
#define In3_name  9
#define TouchInt  38

#define In0    (!digitalRead(In0_name))
#define In1    (!digitalRead(In1_name))
#define In2    (!digitalRead(In2_name))
#define In3    (!digitalRead(In3_name))
#define In4    (!CheckBit(InReg,0))
#define In5    (!CheckBit(InReg,1))
#define In6    (!CheckBit(InReg,2))
#define In7    (!CheckBit(InReg,3))
#define In8    (!CheckBit(InReg,4))
#define In9    (!CheckBit(InReg,5))
#define In10   (!CheckBit(InReg,6))
#define In11   (!CheckBit(InReg,7))
#define Out0_name 44  // PA30
#define setOut0 digitalWrite(Out0_name, LOW)
#define resOut0 digitalWrite(Out0_name, HIGH)
#define Out0 (!digitalRead(Out0_name))
#define setReset ClearBit(OutReg,0)
#define resReset SetBit(OutReg,0)
#define setOut1 SetBit(OutReg,1)
#define resOut1 ClearBit(OutReg,1)
#define Out1    CheckBit(OutReg,1)
#define setOut2 SetBit(OutReg,2)
#define resOut2 ClearBit(OutReg,2)
#define Out2    CheckBit(OutReg,2)
#define setOut3 SetBit(OutReg,3)
#define resOut3 ClearBit(OutReg,3)
#define Out3    CheckBit(OutReg,3)
#define setOut4 SetBit(OutReg,4)
#define resOut4 ClearBit(OutReg,4)
#define Out4    CheckBit(OutReg,4)
#define setOut5 SetBit(OutReg,5)
#define resOut5 ClearBit(OutReg,5)
#define Out5    CheckBit(OutReg,5)
#define setOut6 SetBit(OutReg,6)
#define resOut6 ClearBit(OutReg,6)
#define Out6    CheckBit(OutReg,6)
#define setOut7 SetBit(OutReg,7)
#define resOut7 ClearBit(OutReg,7)
#define Out7    CheckBit(OutReg,7)
#define setLed  digitalWrite(LED_BUILTIN, HIGH) // Yellow front Led
#define resLed  digitalWrite(LED_BUILTIN, LOW)  // Yellow front Led
#define set_Rs485_TxEn  digitalWrite(Rs485_TxEn, HIGH)
#define res_Rs485_TxEn  digitalWrite(Rs485_TxEn, LOW)

// =============================================================================
// 4. Shift-register I/O update and board setup (GEVA Elettronica)
// =============================================================================
void updateIO()  // 74HC595 Output + 74HC165 Input
{
#if latchPinO != latchPinI
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(latchPinI, LOW);
  digitalWrite(latchPinI, HIGH);
  InReg = SPI.transfer(OutReg);
  digitalWrite(latchPinO, LOW);
  digitalWrite(latchPinO, HIGH);
#else
// If there is only one wire as Latch In and Out, it reads the inputs of the
// previous cycle. Call it twice to read the latest Input state.
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  InReg = SPI.transfer(OutReg);
  digitalWrite(latchPinO, LOW);
  digitalWrite(latchPinI, LOW);
  digitalWrite(latchPinO, HIGH);
  digitalWrite(latchPinI, HIGH);
#endif
}


void gevino_io_setup( void ){

//  Assign pins 10 & 11 SERCOM functionality
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
//  Assign pins 3 & 4 SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);
  pinPeripheral(4, PIO_SERCOM_ALT);

  PORT->Group[PORTB].PINCFG[37].bit.DRVSTR = 1;  // More current on output pin Serial-Tx
  PORT->Group[PORTA].PINCFG[10].bit.DRVSTR = 1;  // More current on output pin Serial1-Tx
  PORT->Group[PORTA].PINCFG[18].bit.DRVSTR = 1;  // More current on output pin Serial2-Tx
  PORT->Group[PORTA].PINCFG[8].bit.DRVSTR = 1;   // More current on output pin Serial3-Tx
  PORT->Group[PORTA].PINCFG[22].bit.DRVSTR = 1;  // More current on output pin SDA
  PORT->Group[PORTA].PINCFG[23].bit.DRVSTR = 1;  // More current on output pin SCL
  PORT->Group[PORTB].PINCFG[10].bit.DRVSTR = 1;  // More current on output pin MOSI
  PORT->Group[PORTB].PINCFG[11].bit.DRVSTR = 1;  // More current on output pin SCK
  PORT->Group[PORTA].PINCFG[19].bit.DRVSTR = 1;  // More current on output pin D12 cs-SD
  PORT->Group[PORTA].PINCFG[15].bit.DRVSTR = 1;  // More current on output pin D5 cs-Eth
  PORT->Group[PORTA].PINCFG[15].bit.DRVSTR = 1;  // More current on output pin D45 CH376-CS  PA31
  PORT->Group[PORTA].PINCFG[20].bit.DRVSTR = 1;  // More current on output pin D6 LatchOut PA20
  PORT->Group[PORTA].PINCFG[14].bit.DRVSTR = 1;  // More current on output pin D2 LatchIn  PA14
  PORT->Group[PORTA].PINCFG[3].bit.DRVSTR = 1;   // More current on output pin D42 TFT-CS PA3

  SPI.begin();

#ifdef TestSDpin
  while(1){
    digitalWrite(latchPin, HIGH);
    delay(100);
    digitalWrite(latchPin, LOW);
    digitalWrite(dataPin, HIGH);
    delay(100);
    digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    delay(100);
    digitalWrite(clockPin, LOW);
  }
#endif

// Define Output
  pinMode(Out0_name, OUTPUT);       // Set Out0_name digital pin as Output
  resOut0;
  pinMode(LED_BUILTIN, OUTPUT); // Yellow front Led
  pinMode(ETH_CS, OUTPUT);
  digitalWrite(ETH_CS, HIGH);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  pinMode(CH376_CS, OUTPUT);
  digitalWrite(CH376_CS, HIGH);
  pinMode(CH376_INT, INPUT);
  digitalWrite(CH376_INT, LOW);
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);

// Shift
  pinMode(latchPinI, OUTPUT);
  pinMode(latchPinO, OUTPUT);

  pinMode(TouchInt, OUTPUT);
  digitalWrite(TouchInt, LOW);
  delay(50);
  setReset;
  updateIO();
  delay(300);
  resReset;
  updateIO();
  delay(50);
  pinMode(TouchInt, INPUT_PULLUP);

//// Define Input Pullup
  pinMode(In0_name, INPUT_PULLUP); // In00
  pinMode(In1_name, INPUT_PULLUP); // In01
  pinMode(In2_name, INPUT_PULLUP); // In02
  pinMode(In3_name, INPUT_PULLUP); // In03

  res_Rs485_TxEn;
  pinMode(Rs485_TxEn, OUTPUT);

  analogReadResolution(12);   // ADC resolution to 12-bit (maximum accepted)
  analogWriteResolution(10);  // DAC resolution to 10-bit (maximum accepted)
}

// =============================================================================
// 5. Non-blocking analog polling (GEVA Elettronica)
//    The standard Arduino analogRead() freezes the program for several
//    milliseconds. analogPolling() cycles through the channels without blocking
//    and stores the latest conversions in analogResult[].
// =============================================================================
uint32_t analogResult[6];

void analogPolling(void){
//A0 = PA2 = AIN0
//A1 = PB8 = AIN2
//A2 = PB9 = AIN3
//A3 = PA4 = AIN4
//A4 = PA5 = AIN5
//A5 = PB2 = AIN10
  static byte pin = 2;


  if( ADC->INTFLAG.bit.RESRDY ){				// if conversion is done
    ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;
    switch(pin){
	  case 0:
	    analogResult[0] = ADC->RESULT.reg;	// save conversion value
	    pin = 2;
        ADC->INPUTCTRL.bit.MUXPOS = pin; // Selection new conversion pin
        ADC->SWTRIG.bit.START = 1;            // start new conversion
	    break;
	  case 2:
	    analogResult[1] = ADC->RESULT.reg;	// save conversion value
	    pin = 3;
        ADC->INPUTCTRL.bit.MUXPOS = pin; // Selection new conversion pin
        ADC->SWTRIG.bit.START = 1;            // start new conversion
	    break;
	  case 3:
	    analogResult[2] = ADC->RESULT.reg;	// save conversion value
	    pin = 4;
        ADC->INPUTCTRL.bit.MUXPOS = pin; // Selection new conversion pin
        ADC->SWTRIG.bit.START = 1;            // start new conversion
	  break;
	    case 4:
	    analogResult[3] = ADC->RESULT.reg;	// save conversion value
	    pin = 5;
        ADC->INPUTCTRL.bit.MUXPOS = pin; // Selection new conversion pin
        ADC->SWTRIG.bit.START = 1;            // start new conversion
	    break;
	  case 5:
	    analogResult[4] = ADC->RESULT.reg;	// save conversion value
	    pin = 0;
        ADC->INPUTCTRL.bit.MUXPOS = pin; // Selection new conversion pin
        ADC->SWTRIG.bit.START = 1;            // start new conversion
	    break;
	  }
   }
}

void analogEnable( void ){
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x01;             // Enable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->INPUTCTRL.bit.MUXNEG = ADC_PIN_IOGND; // set back muxneg to internal ground  ADC_PIN_GND / ADC_PIN_IOGND
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->INPUTCTRL.bit.MUXPOS = 0;             // Selection new conversion pin
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;                 // start new conversion
}

// =============================================================================
// 6. Timer class (GEVA Elettronica - Giorgio Evangelista)
//    Simple non-blocking software timer based on millis().
//
//    Behaviour:
//      - While it keeps being reset(), it never fires.
//      - Once the set time elapses without being reset, it fires.
//
//    See the Timer example.
// =============================================================================
class Timer
{
  public:
    Timer( unsigned long _t ){ _Time = _t; _Timer = millis()+_Time; };
    Timer() {_Timer = millis();}
    void reset(void){ _Timer = millis(); }
    void set(void){ _Timer = millis() + _Time; }
    bool stato(void){ return ( millis() - _Timer >= _Time ); }
    bool fronte(void){
	  if ( !ff && ( millis() - _Timer >= _Time )){
		  ff = 1;
		  return 1;
	  }else{
		  if ( millis() - _Timer <= _Time ) ff=0;
		  return 0;
	  }
	}
    void tempo(unsigned long _t){ _Time = _t; _Timer = millis()+_t;}

  private:
    unsigned long _Time;
    unsigned long _Timer;
	bool ff = 0;
};

#endif // _GEVINO_TFT_IO_H_
