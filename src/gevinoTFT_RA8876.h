#pragma GCC diagnostic warning "-Wall"

#ifndef gevinoTFT_RA8876_H
#define gevinoTFT_RA8876_H

#include <Arduino.h>
#include <SPI.h>

#define OffsetBuff 1024*600*2  //1228800
//#define OffsetBuff 0
#define FontChipCS 1   // font chip 0 or 1

#define Pic_Offset (( ENDFLASH + 0x10000 ) &  0xFF0000 )

// Data sheet section 5.2 says maximum SPI clock is 50MHz.
// C:\Users\x-gio\AppData\Local\Arduino15\packages\arduino\hardware\samd\1.8.6\libraries\SPI\spi.h
// ln45  #define SPI_MIN_CLOCK_DIVIDER (uint8_t)(1 + ((F_CPU - 1) / 24000000))
#define RA8876_SPI_SPEED 12000000
//#define RA8876_SPI_SPEED 24000000

#define	BLACK   	0x0000
#define WHITE   	0xFFFF
#define BLUE    	0x001F
#define RED     	0xF800
#define GREEN   	0x07E0
#define CYAN    	0x07FF
#define MAGENTA 	0xF81F
#define YELLOW  	0xFFE0
#define BRIGHT_RED	0xF810	
#define LIGHT_GREY	0x8410  
#define DARK_GREY	0x4208  

typedef struct _font_char_info {
   uint8_t Charwidth;
   uint16_t Offset;
} __attribute__((packed)) FONT_CHAR_INFO;
typedef struct _font_info {
  uint16_t CharacterHeight;
  uint16_t BlockWidth;
  uint32_t FlashStart;
  const FONT_CHAR_INFO *Map;
} __attribute__((packed)) FONT_INFO;

struct SdramInfo
{
  int speed;       // MHz
  int casLatency;  // CAS latency (2 or 3)
  int banks;       // Banks (2 or 4)
  int rowBits;     // Row addressing bits (11-13)
  int colBits;     // Column addressing bits (8-12)
  int refresh;     // Refresh time in microseconds
};

struct DisplayInfo
{
  int      width;     // Display width
  int      height;    // Display height
  
  uint32_t dotClock;  // Pixel clock in kHz
  
  int      hFrontPorch;  // Will be rounded to the nearest multiple of 8
  int      hBackPorch;
  int      hPulseWidth;  // Will be rounded to the nearest multiple of 8
  
  int      vFrontPorch;
  int      vBackPorch;
  int      vPulseWidth;
};

// Data sheet section 6.1.
// Output frequency is: (m_oscClock * (n + 1)) / (2 ** k)
// There is also a PLL parameter named 'm', but it's unclear how its value could ever be non-zero.
//  When it is zero, the divisor is (2 ** 0) = 1, so we simply ignore it.
struct PllParams
{
  uint32_t freq;  // Frequency in kHz
  int      n;     // Multiplier less 1 (range 1..63)
  int      k;     // Divisor power of 2 (range 0..3 for CCLK/MCLK; range 0..7 for SCLK)
};

#define RGB332(r, g, b) (((r) & 0xE0) | (((g) & 0xE0) >> 3) | (((b) & 0xE0) >> 6))
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

//CAL! G4 G3 G2 B7 B6 B5 B4 B3 | R7 R6 R5 R4 R3 G7 G6 G5
#define cal_RGB565(r,g,b) ( ((g & 0x1C) << 11) | ((b & 0xF8) << 5) | ((r & 0xF8) << 0) | ((g & 0xE0) >> 5) )

enum FontSource
{
  RA8876_FONT_SOURCE_INTERNAL,   // CGROM with four 8-bit ISO Latin variants
  RA8876_FONT_SOURCE_EXT_ROM,    // External font ROM chip
  RA8876_FONT_SOURCE_EXT_GEVA
};

enum FontSize
{
  RA8876_FONT_SIZE_16 = 0x00,
  RA8876_FONT_SIZE_24 = 0x01,
  RA8876_FONT_SIZE_32 = 0x02
};

enum FontEncoding
{
  RA8876_FONT_ENCODING_GB2312   = 0x00,  // GB2312 (Simplified Chinese)
  RA8876_FONT_ENCODING_GB18030  = 0x01,  // GB12345/GB18030 (Chinese)
  RA8876_FONT_ENCODING_BIG5     = 0x02,  // Big5 (Traditional Chinese)

  RA8876_FONT_ENCODING_UNICODE  = 0x03,  // Unicode (UCS-2?)

  RA8876_FONT_ENCODING_ASCII    = 0x04,  // ASCII

  RA8876_FONT_ENCODING_UNIJAPAN = 0x05,  // Uni-Japanese (?)
  RA8876_FONT_ENCODING_JIS0208  = 0x06,  // JIS X 0208 (Shift JIS?)

  RA8876_FONT_ENCODING_LGCATH   = 0x07,  // Latin/Greek/Cyrillic/Arabic/Thai/Hebrew (?)

  RA8876_FONT_ENCODING_8859_1   = 0x11,  // ISO 8859-1 (Latin 1)
  RA8876_FONT_ENCODING_8859_2   = 0x12,  // ISO 8859-2 (Latin 2: Eastern European)
  RA8876_FONT_ENCODING_8859_3   = 0x13,  // ISO 8859-3 (Latin 3: South European)
  RA8876_FONT_ENCODING_8859_4   = 0x14,  // ISO 8859-4 (Latin 4: Northern European)
  RA8876_FONT_ENCODING_8859_5   = 0x15,  // ISO 8859-5 (Latin/Cyrillic)
  RA8876_FONT_ENCODING_8859_7   = 0x16,  // ISO 8859-7 (Latin/Greek)
  RA8876_FONT_ENCODING_8859_8   = 0x17,  // ISO 8859-8 (Latin/Hebrew)
  RA8876_FONT_ENCODING_8859_9   = 0x18,  // ISO 8859-9 (Latin 5: Turkish)
  RA8876_FONT_ENCODING_8859_10  = 0x19,  // ISO 8859-10 (Latin 6: Nordic)
  RA8876_FONT_ENCODING_8859_11  = 0x1A,  // ISO 8859-11 (Latin/Thai)
  RA8876_FONT_ENCODING_8859_13  = 0x1B,  // ISO 8859-13 (Latin 7: Baltic Rim)
  RA8876_FONT_ENCODING_8859_14  = 0x1C,  // ISO 8859-14 (Latin 8: Celtic)
  RA8876_FONT_ENCODING_8859_15  = 0x1D,  // ISO 8859-15 (Latin 9: Western European)
  RA8876_FONT_ENCODING_8859_16  = 0x1E   // ISO 8859-16 (Latin 10: South-Eastern European)
};

enum ExternalFontRom
{
  RA8876_FONT_ROM_GT21L16T1W = 0,
  RA8876_FONT_ROM_GT30L16U2W = 1,
  RA8876_FONT_ROM_GT30L24T3Y = 2,
  RA8876_FONT_ROM_GT30L24M1Z = 3,
  RA8876_FONT_ROM_GT30L32S4W = 4,
  RA8876_FONT_ROM_GT20L24F6Y = 5,
  RA8876_FONT_ROM_GT21L24S1W = 6
};

struct ExternalFontRomInfo
{
  bool present;
  int  spiInterface;          // SPI interface that font ROM is connected to (0 or 1)
  int  spiClockDivisor;       // SPI interface clock divisor (2..512 in steps of 2)
  enum ExternalFontRom chip;  // Chip type
};

enum ExternalFontFamily
{
  RA8876_FONT_FAMILY_FIXED = 0,
  RA8876_FONT_FAMILY_ARIAL = 1,
  RA8876_FONT_FAMILY_TIMES = 2,
  RA8876_FONT_FAMILY_FIXED_BOLD = 3
};

typedef uint8_t FontFlags;
#define RA8876_FONT_FLAG_XLAT_FULLWIDTH 0x01  // Translate ASCII to Unicode fullwidth forms


// With SPI, the RA8876 expects an initial byte where the top two bits are meaningful. Bit 7
// is A0, bit 6 is WR#. See data sheet section 7.3.2 and section 19.
// A0: 0 for command/status, 1 for data
// WR#: 0 for write, 1 for read
#define RA8876_DATA_WRITE  0x80
#define RA8876_DATA_READ   0xC0
#define RA8876_CMD_WRITE   0x00
#define RA8876_STATUS_READ 0x40

// Data sheet 19.2: Chip configuration registers
#define RA8876_REG_SRR     0x00  // Software Reset Register
#define RA8876_REG_CCR     0x01  // Chip Configuration Register
#define RA8876_REG_MACR    0x02  // Memory Access Control Register
#define RA8876_REG_ICR     0x03  // Input Control Register
#define RA8876_REG_MRWDP   0x04  // Memory Read/Write Data Port

// Data sheet 19.3: PLL setting registers
#define RA8876_REG_PPLLC1  0x05  // SCLK PLL control register 1
#define RA8876_REG_PPLLC2  0x06  // SCLK PLL control register 2
#define RA8876_REG_MPLLC1  0x07  // MCLK PLL control register 1
#define RA8876_REG_MPLLC2  0x08  // MCLK PLL control register 2
#define RA8876_REG_SPLLC1  0x09  // CCLK PLL control register 1
#define RA8876_REG_SPLLC2  0x0A  // CCLK PLL control register 2

// Data sheet 19.5: LCD display control registers
#define RA8876_REG_MPWCTR  0x10  // Main/PIP Window Control Register
#define RA8876_REG_PIPCDEP 0x11  // PIP Window Color Depth register
#define RA8876_REG_DPCR    0x12  // Display configuration register
#define RA8876_REG_PCSR    0x13  // Panel scan clock and data setting register
#define RA8876_REG_HDWR    0x14  // Horizontal Display Width Register
#define RA8876_REG_HDWFTR  0x15  // Horizontal Display Width Fine Tuning Register
#define RA8876_REG_HNDR    0x16  // Horizontal Non-Display Period Register
#define RA8876_REG_HNDFTR  0x17  // Horizontal Non-Display Period Fine Tuning Register
#define RA8876_REG_HSTR    0x18  // HSYNC start position register
#define RA8876_REG_HPWR    0x19  // HSYNC Pulse Width Register
#define RA8876_REG_VDHR0   0x1A  // Vertical Display Height Register 0
#define RA8876_REG_VDHR1   0x1B  // Vertical Display Height Register 1
#define RA8876_REG_VNDR0   0x1C  // Vertical Non-Display Period Register 0
#define RA8876_REG_VNDR1   0x1D  // Vertical Non-Display Period Register 1
#define RA8876_REG_VSTR    0x1E  // VSYNC start position register
#define RA8876_REG_VPWR    0x1F  // VSYNC pulse width register
#define RA8876_REG_MISA0   0x20  // Main Image Start Address 0
#define RA8876_REG_MISA1   0x21  // Main Image Start Address 1
#define RA8876_REG_MISA2   0x22  // Main Image Start Address 2
#define RA8876_REG_MISA3   0x23  // Main Image Start Address 3
#define RA8876_REG_MIW0    0x24  // Main Image Width 0
#define RA8876_REG_MIW1    0x25  // Main Image Width 1
#define RA8876_REG_MWULX0  0x26  // Main Window Upper-Left X coordinate 0
#define RA8876_REG_MWULX1  0x27  // Main Window Upper-Left X coordinate 1
#define RA8876_REG_MWULY0  0x28  // Main Window Upper-Left Y coordinate 0
#define RA8876_REG_MWULY1  0x29  // Main Window Upper-Left Y coordinate 1

// Data sheet 19.6: Geometric engine control registers
#define RA8876_REG_CVSSA0     0x50  // Canvas Start Address 0
#define RA8876_REG_CVSSA1     0x51  // Canvas Start Address 1
#define RA8876_REG_CVSSA2     0x52  // Canvas Start Address 2
#define RA8876_REG_CVSSA3     0x53  // Canvas Start Address 3
#define RA8876_REG_CVS_IMWTH0 0x54  // Canvas image width 0
#define RA8876_REG_CVS_IMWTH1 0x55  // Canvas image width 1
#define RA8876_REG_AWUL_X0    0x56  // Active Window Upper-Left X coordinate 0
#define RA8876_REG_AWUL_X1    0x57  // Active Window Upper-Left X coordinate 1
#define RA8876_REG_AWUL_Y0    0x58  // Active Window Upper-Left Y coordinate 0
#define RA8876_REG_AWUL_Y1    0x59  // Active Window Upper-Left Y coordinate 1
#define RA8876_REG_AW_WTH0    0x5A  // Active Window Width 0
#define RA8876_REG_AW_WTH1    0x5B  // Active Window Width 1
#define RA8876_REG_AW_HT0     0x5C  // Active Window Height 0
#define RA8876_REG_AW_HT1     0x5D  // Active Window Height 1
#define RA8876_REG_AW_COLOR   0x5E  // Color Depth of canvas & active window
#define RA8876_REG_CURH0      0x5F  // Graphic read/write horizontal position 0
#define RA8876_REG_CURH1      0x60  // Graphic read/write horizontal position 1
#define RA8876_REG_CURV0      0x61  // Graphic read/write vertical position 0
#define RA8876_REG_CURV1      0x62  // Graphic read/write vertical position 1
#define RA8876_REG_F_CURX0    0x63  // Text cursor X-coordinate register 0
#define RA8876_REG_F_CURX1    0x64  // Text cursor X-coordinate register 1
#define RA8876_REG_F_CURY0    0x65  // Text cursor Y-coordinate register 0
#define RA8876_REG_F_CURY1    0x66  // Text cursor Y-coordinate register 1

#define RA8876_REG_DCR0       0x67  // Draw shape control register 0

#define RA8876_REG_DLHSR0     0x68  // Draw shape point 1 X coordinate register 0
#define RA8876_REG_DLHSR1     0x69  // Draw shape point 1 X coordinate register 1
#define RA8876_REG_DLVSR0     0x6A  // Draw shape point 1 Y coordinate register 0
#define RA8876_REG_DLVSR1     0x6B  // Draw shape point 1 Y coordinate register 1

#define RA8876_REG_DLHER0     0x6C  // Draw shape point 2 X coordinate register 0
#define RA8876_REG_DLHER1     0x6D  // Draw shape point 2 X coordinate register 1
#define RA8876_REG_DLVER0     0x6E  // Draw shape point 2 Y coordinate register 0
#define RA8876_REG_DLVER1     0x6F  // Draw shape point 2 Y coordinate register 1

#define RA8876_REG_DTPH0      0x70  // Draw shape point 3 X coordinate register 0
#define RA8876_REG_DTPH1      0x71  // Draw shape point 3 X coordinate register 1
#define RA8876_REG_DTPV0      0x72  // Draw shape point 3 Y coordinate register 0
#define RA8876_REG_DTPV1      0x73  // Draw shape point 3 Y coordinate register 1

#define RA8876_REG_DCR1       0x76  // Draw shape control register 1

#define RA8876_REG_ELL_A0     0x77  // Draw ellipse major radius 0
#define RA8876_REG_ELL_A1     0x78  // Draw ellipse major radius 1
#define RA8876_REG_ELL_B0     0x79  // Draw ellipse minor radius 0
#define RA8876_REG_ELL_B1     0x7A  // Draw ellipse minor radius 1

#define RA8876_REG_DEHR0      0x7B  // Draw ellipse centre X coordinate register 0
#define RA8876_REG_DEHR1      0x7C  // Draw ellipse centre X coordinate register 1
#define RA8876_REG_DEVR0      0x7D  // Draw ellipse centre Y coordinate register 0
#define RA8876_REG_DEVR1      0x7E  // Draw ellipse centre Y coordinate register 1

// Data sheet 19.7: PWM timer control registers
#define RA8876_REG_PSCLR      0x84  // PWM prescaler register
#define RA8876_REG_PMUXR      0x85  // PWM clock mux register
#define RA8876_REG_PCFGR      0x86  // PWM configuration register

// Data sheet 19.9: Serial flash & SPI master control registers
#define RA8876_REG_SFL_CTRL   0xB7  // Serial flash/ROM control register
#define RA8876_REG_SPI_DIVSOR 0xBB  // SPI clock period

// Data sheet 19.10: Text engine
#define RA8876_REG_CCR0       0xCC  // Character Control Register 0
#define RA8876_REG_CCR1       0xCD  // Character Control Register 1
#define RA8876_REG_GTFNT_SEL  0xCE  // Genitop character ROM select
#define RA8876_REG_GTFNT_CR   0xCF  // Genitop character ROM control register

#define RA8876_REG_FLDR       0xD0  // Chracter line gap register
#define RA8876_REG_F2FSSR     0xD1  // Chracter to character space setting register
#define RA8876_REG_FGCR       0xD2  // Foreground colour register - red
#define RA8876_REG_FGCG       0xD3  // Foreground colour register - green
#define RA8876_REG_FGCB       0xD4  // Foreground colour register - blue

// Data sheet 19.12: SDRAM control registers
#define RA8876_REG_SDRAR         0xE0  // SDRAM attribute register
#define RA8876_REG_SDRMD         0xE1  // SDRAM mode & extended mode register
#define RA8876_REG_SDR_REF_ITVL0 0xE2  // SDRAM auto refresh interval 0
#define RA8876_REG_SDR_REF_ITVL1 0xE3  // SDRAM auto refresh interval 1
#define RA8876_REG_SDRCR         0xE4  // SDRAM control register


#ifndef pic_def
#define pic_def
typedef struct _info
{
  unsigned short number;
  unsigned short img_width;
  unsigned short img_height;
  unsigned long img_size;
  unsigned long start_addr;
}INFO;
#endif
#define pic(a,b,c) pic_b(&BINARY_INFO[a],b,c)
#define pic_part(a,b,c,d,e,f) pic_c(&BINARY_INFO[a],b,c,d,e,f)

class RA8876 : public Print
{
private:

public:

  int m_csPin;
  int m_resetPin;

  int m_width;
  int m_height;
  int m_depth;
  int X_Cursor = 0;
  int Y_Cursor = 0;
//  uint8_t CharSpacing = 8;
  uint8_t p = 0;        // Previews Caracter
  bool ClearBG = 1;
    
  const FONT_INFO *FontNow;

  uint32_t m_oscClock;   // OSC clock (external crystal) frequency in kHz

  PllParams m_memPll;   // MCLK (memory) PLL parameters
  PllParams m_corePll;  // CCLK (core) PLL parameters
  PllParams m_scanPll;  // SCLK (LCD panel scan) PLL parameters

  SPISettings m_spiSettings;

  SdramInfo *m_sdramInfo;

  DisplayInfo *m_displayInfo;

  ExternalFontRomInfo m_fontRomInfo;

  uint16_t m_textColor;
  uint16_t m_BGtextColor;
  int      m_textScaleX = 1;
  int      m_textScaleY = 1;

  enum FontSource m_fontSource;
  enum FontSize   m_fontSize;
  FontFlags       m_fontFlags;

  void hardReset(void);
  void softReset(void);

  void writeCmd(uint8_t x);
  void writeData(uint8_t x);
  uint8_t readData(void);
  uint8_t readStatus(void);

  void writeReg(uint8_t reg, uint8_t x);
  void writeReg16(uint8_t reg, uint16_t x);
  uint8_t readReg(uint8_t reg);
  uint16_t readReg16(uint8_t reg);

  inline void waitWriteFifo(void) { while (readStatus() & 0x80); };
  inline void waitTaskBusy(void) { while (readStatus() & 0x08); };

  bool calcPllParams(uint32_t targetFreq, int kMax, PllParams *pll);
  bool calcClocks(void);
  void dumpClocks(void);

  bool initPLL(void);
  bool initMemory(SdramInfo *info);
  bool initDisplay();

  // Font utils
  uint8_t internalFontEncoding(enum FontEncoding enc);

  // Text/graphics mode
  void setTextMode(void);
  void setGraphicsMode(void);

  // Low-level shapes
  void drawTwoPointShape(int x1, int y1, int x2, int y2, uint16_t color, uint8_t reg, uint8_t cmd);  // drawLine, drawRect, fillRect
  void drawThreePointShape(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color, uint8_t reg, uint8_t cmd);  // drawTriangle, fillTriangle
  void drawEllipseShape(int x, int y, int xrad, int yrad, uint16_t color, uint8_t cmd);  // drawCircle, fillCircle

  
  
  
  RA8876(int csPin, int resetPin = 0);

  // Init
  bool init();   // 0 = H   3 = V
  void initExternalFontRom(int spiIf, enum ExternalFontRom chip);

  // SD -> serial-flash auto-loader. init() calls copySDtoFlash() at the end: if
  // the SD card holds font.bin / All_76_Pic_65K.bin they are (re)written into the
  // display serial flash. Without an SD card (or those files) it does nothing, so
  // it is safe to run on every boot. (Needs the SdFat library.)
  void copySDtoFlash(void);
  void writeFlash(unsigned long flash_addr, const char *file, byte erase); // erase: 0=none, 1=all, 2=pic area
  unsigned char flashReadStatus(void);

  // Dimensions
  int getWidth() { return m_width; };
  int getHeight() { return m_height; };

  // Test
  void colorBarTest(bool enabled);

  // Drawing
  void drawPixel(int x, int y, uint16_t color);
  void drawLine(int x1, int y1, int x2, int y2, uint16_t color) { drawTwoPointShape(x1, y1, x2, y2, color, RA8876_REG_DCR0, 0x80); };
  void drawRect(int x1, int y1, int x2, int y2, uint16_t color) { drawTwoPointShape(x1, y1, x2, y2, color, RA8876_REG_DCR1, 0xA0); };
  void fillRect(int x1, int y1, int x2, int y2, uint16_t color) { drawTwoPointShape(x1, y1, x2, y2, color, RA8876_REG_DCR1, 0xE0); };

  void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) { drawThreePointShape(x1, y1, x2, y2, x3, y3, color, RA8876_REG_DCR0, 0xA2); };
  void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) { drawThreePointShape(x1, y1, x2, y2, x3, y3, color, RA8876_REG_DCR0, 0xE2); };

  void drawCircle(int x, int y, int radius, uint16_t color) { drawEllipseShape(x, y, radius, radius, color, 0x80); };
  void fillCircle(int x, int y, int radius, uint16_t color) { drawEllipseShape(x, y, radius, radius, color, 0xC0); };

  void clearScreen(uint16_t color);

  // Text cursor
  void setCursor(int x, int y);
  int getCursorX(void);
  int getCursorY(void);  

  // Text
  void setFont(const FONT_INFO *FontTo);
  void selectInternalFont(enum FontSize size, enum FontEncoding enc = RA8876_FONT_ENCODING_8859_1);
  void selectExternalFont(enum ExternalFontFamily family, enum FontSize size, enum FontEncoding enc, FontFlags flags = 0);
  int getTextSizeY(void);
  void setTextColor(uint16_t color);		// Clear BG
  void setTextColor(uint16_t c, uint16_t bg);  
  void setTextScale(int scale) { setTextScale(scale, scale); };
  void setTextScale(int xScale, int yScale);
  // void putChar(char c) { putChars(&c, 1); };
  // void putChars(const char *buffer, size_t size);
  // void putChar16(uint16_t c) { putChars16(&c, 1); };
  // void putChars16(const uint16_t *buffer, unsigned int count);
  void getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
  void charBounds( uint8_t c, uint16_t *w);

  // Internal for Print class
  virtual size_t write(uint8_t c) { return write(&c, 1); };
  virtual size_t write(const uint8_t *buffer, size_t size);
  void putChar( unsigned char cr );
  void FontCanvasLoad( void );

//-----------------   API from RAIO  --------------------------------
void pic_b ( const INFO* myInfo, uint32_t X1, uint32_t Y1);	// Enum Image, X , Y
void pic_c ( const INFO* myInfo, uint32_t X1, uint32_t Y1, uint32_t X2, uint32_t W2, uint32_t H2);	// Enum Image, X , Y, (X, Y ,image part)
void DMA_24bit(unsigned char SCS,unsigned char Clk,uint32_t X1,uint32_t Y1,uint32_t X_W,uint32_t Y_H,uint32_t P_W,unsigned long Addr);
void PWM0(unsigned char on_off, unsigned char Clock_Divided, unsigned char Prescalar, unsigned short Count_Buffer,unsigned short Compare_Buffer);

//-----------------   FROM RAIO  --------------------------------
    //--  from RAIO API
void RA8876_HW_Reset(void);
void System_Check_Temp(void);
void RA8876_Initial(void);
void Set_LCD_Panel(void);
void Set_Serial_Flash_IF(void);
//void LCD_RegisterWrite(unsigned char Cmd,unsigned char Data);
//unsigned char LCD_RegisterRead(unsigned char Cmd);
void Check_Mem_WR_FIFO_not_Full(void);
void Check_Mem_WR_FIFO_Empty(void);
void Check_Mem_RD_FIFO_not_Full(void);
void Check_Mem_RD_FIFO_Full(void);
void Check_Mem_RD_FIFO_not_Empty(void);
void Check_2D_Busy(void);
void Check_SDRAM_Ready(void);
unsigned char Power_Saving_Status(void);
void Check_IC_ready(void);
void Check_Power_is_Saving(void);
void Check_NO_Interrupt(void);
void Check_Interrupt_Occur(void);
void RA8876_SW_Reset(void);
void Enable_PLL(void);
void Sent_XnWAIT_Status_When_CS_Low(void);
void Sent_XnWAIT_Status_At_Any_Time(void);
void TFT_24bit(void);//RA8876 only
void TFT_18bit(void);//RA8876 only
void TFT_16bit(void);//RA8876 only
void Without_TFT(void);//RA8876 only
void Key_Scan_Enable(void);
void Key_Scan_Disable(void);
void RA8876_I2CM_Enable(void);
void RA8876_I2CM_Disable(void);
void Enable_SFlash_SPI(void);
void Disable_SFlash_SPI(void);
void Host_Bus_8bit(void);
void Host_Bus_16bit(void);
void Data_Format_8b_8bpp(void);
void Data_Format_8b_16bpp(void);
void Data_Format_8b_24bpp(void);
void Data_Format_16b_8bpp(void);
void Data_Format_16b_16bpp(void);
void Data_Format_16b_24bpp_mode1(void);
void Data_Format_16b_24bpp_mode2(void);
void MemRead_Left_Right_Top_Down(void);
void MemRead_Right_Left_Top_Down(void);
void MemRead_Top_Down_Left_Right(void);
void MemRead_Down_Top_Left_Right(void);
void MemWrite_Left_Right_Top_Down(void);
void MemWrite_Right_Left_Top_Down(void);
void MemWrite_Top_Down_Left_Right(void);
void MemWrite_Down_Top_Left_Right(void);
void Interrupt_Active_Low(void);
void Interrupt_Active_High(void);
void ExtInterrupt_Debounce(void);
void ExtInterrupt_Nodebounce(void);
void ExtInterrupt_Input_Low_Level_Trigger(void);
void ExtInterrupt_Input_High_Level_Trigger(void);
void ExtInterrupt_Input_Falling_Edge_Trigger(void);
void ExtInterrupt_Input_Rising_Edge_Trigger(void);
void LVDS_Format1(void);//RA8877 only
void LVDS_Format2(void);//RA8877 only
void Graphic_Mode(void);
void Text_Mode(void);
void Memory_Select_SDRAM(void);
void Memory_Select_Gamma_Table(void);
void Memory_Select_Graphic_Cursor_RAM(void);
void Memory_Select_Color_Palette_RAM(void);
void RA8876_PLL (void);//Set PLL 
void Enable_Resume_Interrupt(void);
void Disable_Resume_Interrupt(void);
void Enable_ExtInterrupt_Input(void);
void Disable_ExtInterrupt_Input(void);
void Enable_I2CM_Interrupt(void);
void Disable_I2CM_Interrupt(void);
void Enable_Vsync_Interrupt(void);
void Disable_Vsync_Interrupt(void);
void Enable_KeyScan_Interrupt(void);
void Disable_KeyScan_Interrupt(void);
void Enable_DMA_Draw_BTE_Interrupt(void);
void Disable_DMA_Draw_BTE_Interrupt(void);
void Enable_PWM1_Interrupt(void);
void Disable_PWM1_Interrupt(void);
void Enable_PWM0_Interrupt(void);
void Disable_PWM0_Interrupt(void);
unsigned char Read_Interrupt_status(void);
void Check_Vsync_finished(void);
void Clear_Resume_Interrupt_Flag(void);
void Clear_ExtInterrupt_Input_Flag(void);
void Clear_I2CM_Interrupt_Flag(void);
void Clear_Vsync_Interrupt_Flag(void);
void Clear_KeyScan_Interrupt_Flag(void);
void Clear_DMA_Draw_BTE_Interrupt_Flag(void);
void Clear_PWM1_Interrupt_Flag(void);
void Clear_PWM0_Interrupt_Flag(void);
void XnINTR_Mask_Resume_Interrupt_Flag(void);
void XnINTR_Mask_ExtInterrupt_Input_Flag(void);
void XnINTR_Mask_I2CM_Interrupt_Flag(void);
void XnINTR_Mask_Vsync_Interrupt_Flag(void);
void XnINTR_Mask_KeyScan_Interrupt_Flag(void);
void XnINTR_Mask_DMA_Draw_BTE_Interrupt_Flag(void);
void XnINTR_Mask_PWM1_Interrupt_Flag(void);
void XnINTR_Mask_PWM0_Interrupt_Flag(void);
void XnINTR_Unmask_Resume_Interrupt_Flag(void);
void XnINTR_Unmask_ExtInterrupt_Input_Flag(void);
void XnINTR_Unmask_I2CM_Interrupt_Flag(void);
void XnINTR_Unmask_Vsync_Interrupt_Flag(void);
void XnINTR_Unmask_KeyScan_Interrupt_Flag(void);
void XnINTR_Unmask_DMA_Draw_BTE_Interrupt_Flag(void);
void XnINTR_Unmask_PWM1_Interrupt_Flag(void);
void XnINTR_Unmask_PWM0_Interrupt_Flag(void);
void Enable_GPIOF_PullUp(void);
void Enable_GPIOE_PullUp(void);
void Enable_GPIOD_PullUp(void);
void Enable_GPIOC_PullUp(void);
void Enable_XDB15_8_PullUp(void);
void Enable_XDB7_0_PullUp(void);
void Disable_GPIOF_PullUp(void);
void Disable_GPIOE_PullUp(void);
void Disable_GPIOD_PullUp(void);
void Disable_GPIOC_PullUp(void);
void Disable_XDB15_8_PullUp(void);
void Disable_XDB7_0_PullUp(void);
void XPDAT18_Set_GPIO_D7(void);
void XPDAT18_Set_KOUT4(void);
void XPDAT17_Set_GPIO_D5(void);
void XPDAT17_Set_KOUT2(void);
void XPDAT16_Set_GPIO_D4(void);
void XPDAT16_Set_KOUT1(void);
void XPDAT9_Set_GPIO_D3(void);
void XPDAT9_Set_KOUT3(void);
void XPDAT8_Set_GPIO_D2(void);
void XPDAT8_Set_KIN3(void);
void XPDAT2_Set_GPIO_D6(void);
void XPDAT2_Set_KIN4(void);
void XPDAT1_Set_GPIO_D1(void);
void XPDAT1_Set_KIN2(void);
void XPDAT0_Set_GPIO_D0(void);
void XPDAT0_Set_KIN1(void);
void Enable_PIP1(void);
void Disable_PIP1(void);
void Enable_PIP2(void);
void Disable_PIP2(void);
void Select_PIP1_Parameter(void);
void Select_PIP2_Parameter(void);
void Select_Main_Window_8bpp(void);
void Select_Main_Window_16bpp(void);
void Select_Main_Window_24bpp(void);
void Select_LCD_Sync_Mode(void);
void Select_LCD_DE_Mode(void);
void Select_PIP1_Window_8bpp(void);
void Select_PIP1_Window_16bpp(void);
void Select_PIP1_Window_24bpp(void);
void Select_PIP2_Window_8bpp(void);
void Select_PIP2_Window_16bpp(void);
void Select_PIP2_Window_24bpp(void);
void PCLK_Rising(void);
void PCLK_Falling(void);
void Display_ON(void);
void Display_OFF(void);
void Color_Bar_ON(void);
void Color_Bar_OFF(void);
void VSCAN_T_to_B(void);
void VSCAN_B_to_T(void);
void PDATA_Set_RGB(void);
void PDATA_Set_RBG(void);
void PDATA_Set_GRB(void);
void PDATA_Set_GBR(void);
void PDATA_Set_BRG(void);
void PDATA_Set_BGR(void);
void PDATA_Set_Gray(void);
void PDATA_IDLE_STATE(void);
void HSYNC_Low_Active(void);
void HSYNC_High_Active(void);
void VSYNC_Low_Active(void);
void VSYNC_High_Active(void);
void DE_Low_Active(void);
void DE_High_Active(void);
void Idle_DE_Low(void);
void Idle_DE_High(void);
void Idle_PCLK_Low(void);
void Idle_PCLK_High(void);
void Idle_PDAT_Low(void);
void Idle_PDAT_High(void);
void Idle_HSYNC_Low(void);
void Idle_HSYNC_High(void);
void Idle_VSYNC_Low(void);
void Idle_VSYNC_High(void);
void LCD_HorizontalWidth_VerticalHeight(unsigned short WX,unsigned short HY);
void LCD_Horizontal_Non_Display(unsigned short WX);
void LCD_HSYNC_Start_Position(unsigned short WX);
void LCD_HSYNC_Pulse_Width(unsigned short WX);
void LCD_Vertical_Non_Display(unsigned short HY);
void LCD_VSYNC_Start_Position(unsigned short HY);
void LCD_VSYNC_Pulse_Width(unsigned short HY);
void Main_Image_Start_Address(unsigned long Addr);
void Main_Image_Width(unsigned short WX);              
void Main_Window_Start_XY(unsigned short WX,unsigned short HY); 
void PIP_Display_Start_XY(unsigned short WX,unsigned short HY);
void PIP_Image_Start_Address(unsigned long Addr);
void PIP_Image_Width(unsigned short WX);
void PIP_Window_Image_Start_XY(unsigned short WX,unsigned short HY);
void PIP_Window_Width_Height(unsigned short WX,unsigned short HY);
void Enable_Gamma_Correction(void);
void Disable_Gamma_Correction(void);
void Gamma_Table_for_Blue(void);
void Gamma_Table_for_Green(void);
void Gamma_Table_for_Red(void);
void Enable_Graphic_Cursor(void);
void Disable_Graphic_Cursor(void);
void Select_Graphic_Cursor_1(void);
void Select_Graphic_Cursor_2(void);
void Select_Graphic_Cursor_3(void);
void Select_Graphic_Cursor_4(void);
void Enable_Text_Cursor(void);
void Disable_Text_Cursor(void);
void Enable_Text_Cursor_Blinking(void);
void Disable_Text_Cursor_Blinking(void);
void Blinking_Time_Frames(unsigned char temp);
void Text_Cursor_H_V(unsigned short WX,unsigned short HY);
void Graphic_Cursor_XY(unsigned short WX,unsigned short HY);
void Set_Graphic_Cursor_Color_1(unsigned char temp);
void Set_Graphic_Cursor_Color_2(unsigned char temp);
void Canvas_Image_Start_address(unsigned long Addr);
void Canvas_image_width(unsigned short WX);
void Active_Window_XY(unsigned short WX,unsigned short HY);
void Active_Window_WH(unsigned short WX,unsigned short HY);
void Memory_XY_Mode(void);
void Memory_Linear_Mode(void);
void Memory_8bpp_Mode(void);
void Memory_16bpp_Mode(void);
void Memory_24bpp_Mode(void);
void Goto_Pixel_XY(unsigned short WX,unsigned short HY);
void Goto_Linear_Addr(unsigned long Addr);
void Goto_Text_XY(unsigned short WX,unsigned short HY);
void Start_Line(void);
void Start_Triangle(void);
void Start_Triangle_Fill(void);
void Line_Start_XY(unsigned short WX,unsigned short HY);
void Line_End_XY(unsigned short WX,unsigned short HY);
void Triangle_Point1_XY(unsigned short WX,unsigned short HY);
void Triangle_Point2_XY(unsigned short WX,unsigned short HY);
void Triangle_Point3_XY (unsigned short WX,unsigned short HY);
void Square_Start_XY(unsigned short WX,unsigned short HY);
void Square_End_XY(unsigned short WX,unsigned short HY);
void Start_Circle_or_Ellipse(void);
void Start_Circle_or_Ellipse_Fill(void);
void Start_Left_Down_Curve(void);
void Start_Left_Up_Curve(void);
void Start_Right_Up_Curve(void);
void Start_Right_Down_Curve(void);
void Start_Left_Down_Curve_Fill(void);
void Start_Left_Up_Curve_Fill(void);
void Start_Right_Up_Curve_Fill(void);
void Start_Right_Down_Curve_Fill(void);
void Start_Square(void);
void Start_Square_Fill(void);
void Start_Circle_Square(void);
void Start_Circle_Square_Fill(void);
void Circle_Center_XY(unsigned short WX,unsigned short HY);
void Ellipse_Center_XY(unsigned short WX,unsigned short HY);
void Circle_Radius_R(unsigned short WX);
void Ellipse_Radius_RxRy(unsigned short WX,unsigned short HY);
void Circle_Square_Radius_RxRy(unsigned short WX,unsigned short HY);
void Set_PWM_Prescaler_1_to_256(unsigned short WX);
void Select_PWM1_Clock_Divided_By_1(void);
void Select_PWM1_Clock_Divided_By_2(void);
void Select_PWM1_Clock_Divided_By_4(void);
void Select_PWM1_Clock_Divided_By_8(void);
void Select_PWM0_Clock_Divided_By_1(void);
void Select_PWM0_Clock_Divided_By_2(void);
void Select_PWM0_Clock_Divided_By_4(void);
void Select_PWM0_Clock_Divided_By_8(void);
void Select_PWM1_is_ErrorFlag(void);
void Select_PWM1(void);
void Select_PWM1_is_Osc_Clock(void);
void Select_PWM0_is_GPIO_C7(void);
void Select_PWM0(void);
void Select_PWM0_is_Core_Clock(void);
void Enable_PWM1_Inverter(void);
void Disable_PWM1_Inverter(void);
void Auto_Reload_PWM1(void);
void One_Shot_PWM1(void);
void Start_PWM1(void);
void Stop_PWM1(void);
void Enable_PWM0_Dead_Zone(void);
void Disable_PWM0_Dead_Zone(void);
void Enable_PWM0_Inverter(void);
void Disable_PWM0_Inverter(void);
void Auto_Reload_PWM0(void);
void One_Shot_PWM0(void);
void Start_PWM0(void);
void Stop_PWM0(void);
void Set_Timer0_Dead_Zone_Length(unsigned char temp);
void Set_Timer0_Compare_Buffer(unsigned short WX);
void Set_Timer0_Count_Buffer(unsigned short WX);
void Set_Timer1_Compare_Buffer(unsigned short WX);
void Set_Timer1_Count_Buffer(unsigned short WX);
void BTE_Enable(void);
void BTE_Disable(void);
void Check_BTE_Busy(void);
void Pattern_Format_8X8(void);
void Pattern_Format_16X16(void);
void BTE_ROP_Code(unsigned char setx);
void BTE_Operation_Code(unsigned char setx);
void BTE_S0_Color_8bpp(void);
void BTE_S0_Color_16bpp(void);
void BTE_S0_Color_24bpp(void);
void BTE_S1_Color_8bpp(void);
void BTE_S1_Color_16bpp(void);
void BTE_S1_Color_24bpp(void);
void BTE_S1_Color_Constant(void);
void BTE_S1_Color_8bit_Alpha(void);
void BTE_S1_Color_16bit_Alpha(void);
void BTE_Destination_Color_8bpp(void);
void BTE_Destination_Color_16bpp(void);
void BTE_Destination_Color_24bpp(void);
void BTE_S0_Memory_Start_Address(unsigned long Addr); 
void BTE_S0_Image_Width(unsigned short WX); 
void BTE_S0_Window_Start_XY(unsigned short WX,unsigned short HY); 
void BTE_S1_Memory_Start_Address(unsigned long Addr);
void S1_Constant_color_256(unsigned char temp);
void S1_Constant_color_65k(unsigned short temp);
void S1_Constant_color_16M(unsigned long temp); 
void BTE_S1_Image_Width(unsigned short WX); 
void BTE_S1_Window_Start_XY(unsigned short WX,unsigned short HY); 
void BTE_Destination_Memory_Start_Address(unsigned long Addr);  
void BTE_Destination_Image_Width(unsigned short WX);  
void BTE_Destination_Window_Start_XY(unsigned short WX,unsigned short HY);  
void BTE_Window_Size(unsigned short WX, unsigned short HY);
void BTE_Alpha_Blending_Effect(unsigned char temp);
void Start_SFI_DMA(void);
void Check_Busy_SFI_DMA(void);
void Select_SFI_0(void);
void Select_SFI_1(void);
void Select_SFI_Font_Mode(void);
void Select_SFI_DMA_Mode(void);
void Select_SFI_24bit_Address(void);
void Select_SFI_32bit_Address(void);
void Select_standard_SPI_Mode0_or_Mode3(void);
void Select_RA8875_SPI_Mode0_and_Mode3(void);
void Select_SFI_Single_Mode_Dummy_0T_03h(void);
void Select_SFI_Single_Mode_Dummy_8T_0Bh(void);
void Select_SFI_Single_Mode_Dummy_16T_1Bh(void);
void Select_SFI_Dual_Mode_Dummy_8T_3Bh(void);
void Select_SFI_Dual_Mode_Dummy_4T_BBh(void);
unsigned char SPI_Master_FIFO_Data_Put(unsigned char Data);
unsigned char SPI_Master_FIFO_Data_Get(void);
void Enable_SPI_Master_Interrupt(void);
void Disable_SPI_Master_Interrupt(void);
void Select_nSS_drive_on_xnsfcs0(void);
void Select_nSS_drive_on_xnsfcs1(void);
void nSS_Inactive(void);
void nSS_Active(void);
void Mask_FIFO_overflow_error_Interrupt(void) ;
void Unmask_FIFO_overflow_error_Interrupt(void);
void Mask_EMTIRQEN_Interrupt(void);
void Unmask_EMTIRQEN_Interrupt(void);
void Reset_CPOL(void);
void Set_CPOL(void);
void Reset_CPHA(void);
void Set_CPHA(void);
unsigned char Tx_FIFO_Empty_Flag(void);
unsigned char Tx_FIFO_Full_Flag(void);
unsigned char Rx_FIFO_Empty_Flag(void);
unsigned char Rx_FIFO_full_flag(void);
unsigned char OVFI_Flag(void);
void Clear_OVFI_Flag(void);
unsigned char EMTI_Flag(void);
void Clear_EMTI_Flag(void);
void SPI_Clock_Period(unsigned char temp);
void SFI_DMA_Source_Start_Address(unsigned long Addr);
void SFI_DMA_Destination_Start_Address(unsigned long Addr);
void SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY);
void SFI_DMA_Destination_Width(unsigned short WX);
void SFI_DMA_Transfer_Number(unsigned long Addr);
void SFI_DMA_Transfer_Width_Height(unsigned short WX,unsigned short HY);
void SFI_DMA_Source_Width(unsigned short WX);
void Font_Select_UserDefine_Mode(void);
void CGROM_Select_Internal_CGROM(void);
void CGROM_Select_Genitop_FontROM(void);
void Font_Select_8x16_16x16(void);
void Font_Select_12x24_24x24(void);
void Font_Select_16x32_32x32(void);
void Internal_CGROM_Select_ISOIEC8859_1(void);
void Internal_CGROM_Select_ISOIEC8859_2(void);
void Internal_CGROM_Select_ISOIEC8859_4(void);
void Internal_CGROM_Select_ISOIEC8859_5(void);
void Enable_Font_Alignment(void);
void Disable_Font_Alignment(void);
void Font_Background_select_Original_Canvas(void);
void Font_Background_select_Color(void);
void Font_0_degree(void);
void Font_90_degree(void);
void Font_Width_X1(void);
void Font_Width_X2(void);
void Font_Width_X3(void);
void Font_Width_X4(void);
void Font_Height_X1(void);
void Font_Height_X2(void);
void Font_Height_X3(void);
void Font_Height_X4(void);
void GTFont_Select_GT21L16T1W(void);
void GTFont_Select_GT30L16U2W(void);
void GTFont_Select_GT30L24T3Y(void);
void GTFont_Select_GT30L24M1Z(void);
void GTFont_Select_GT30L32S4W(void);
void GTFont_Select_GT20L24F6Y(void);
void GTFont_Select_GT21L24S1W(void);
void Set_GTFont_Decoder(unsigned char temp);
void Font_Line_Distance(unsigned char temp);
void Set_Font_to_Font_Width(unsigned char temp);
void Foreground_RGB(unsigned char ,unsigned char ,unsigned char );
void Foreground_color_256(unsigned char temp);
void Foreground_color_65k(unsigned short temp);
void Foreground_color_16M(unsigned long temp);
void Background_RGB(unsigned char ,unsigned char ,unsigned char );
void Background_color_256(unsigned char temp);
void Background_color_65k(unsigned short temp);
void Background_color_16M(unsigned long temp);
void CGRAM_Start_address(unsigned long Addr);
void Power_Normal_Mode(void);
void Power_Saving_Standby_Mode(void);
void Power_Saving_Suspend_Mode(void);
void Power_Saving_Sleep_Mode(void);
void RA8876_SDRAM_initial(void);//Set SDRAM 
void RA8876_I2CM_Clock_Prescale(unsigned short WX);
void RA8876_I2CM_Transmit_Data(unsigned char temp);
unsigned char RA8876_I2CM_Receiver_Data(void);
void RA8876_I2CM_Write_With_Start(void);
void RA8876_I2CM_Stop(void); 
void RA8876_I2CM_Read_With_Ack(void);
void RA8876_I2CM_Read_With_Nack(void);
void RA8876_I2CM_Write(void);
unsigned char RA8876_I2CM_Check_Slave_ACK(void);
unsigned char RA8876_I2CM_Bus_Busy(void);
unsigned char RA8876_I2CM_transmit_Progress(void);
unsigned char RA8876_I2CM_Arbitration(void);
void Set_GPIO_A_In_Out(unsigned char temp);
void Write_GPIO_A_7_0(unsigned char temp);
unsigned char Read_GPIO_A_7_0(void);
void Write_GPIO_B_7_4(unsigned char temp);
unsigned char Read_GPIO_B_7_0(void);
void Set_GPIO_C_In_Out(unsigned char temp);
void Write_GPIO_C_7_0(unsigned char temp);
unsigned char Read_GPIO_C_7_0(void);
void Set_GPIO_D_In_Out(unsigned char temp);
void Write_GPIO_D_7_0(unsigned char temp);
unsigned char Read_GPIO_D_7_0(void);
void Set_GPIO_E_In_Out(unsigned char temp);
void Write_GPIO_E_7_0(unsigned char temp);
unsigned char Read_GPIO_E_7_0(void);
void Set_GPIO_F_In_Out(unsigned char temp);
void Write_GPIO_F_7_0(unsigned char temp);
unsigned char Read_GPIO_F_7_0(void);
void Long_Key_enable(void);
void Key_Scan_Freg(unsigned char temp); //set bit2~0 
void Key_Scan_Wakeup_Function_Enable(void);
void Long_Key_Timing_Adjustment(unsigned char setx);//set bit5~3
unsigned char Numbers_of_Key_Hit(void);
unsigned char Read_Key_Strobe_Data_0(void);
unsigned char Read_Key_Strobe_Data_1(void);
unsigned char Read_Key_Strobe_Data_2(void);
unsigned char Chk_Key_Press(void);

};


#endif
