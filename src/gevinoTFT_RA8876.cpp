#pragma GCC diagnostic warning "-Wall"

#include "gevinoTFT_RA8876.h"
#include "fontEXT.h"
#include <SdFat.h>      // copySDtoFlash(): auto-load font/image .bin from SD at the end of init()

SdramInfo defaultSdramInfo =
{
  120, // 120 MHz
  3,   // CAS latency 3
  4,   // 4 banks
  12,  // 12-bit row addresses
  8,   // 9-bit column addresses //CAL! was 8
  64   // 64 millisecond refresh time
};

DisplayInfo defaultDisplayInfo =
{
  1024,   // Display width
  600,    // Display height
  50000,  // Pixel clock in kHz

  160,    // Horizontal front porch
  160,    // Horizontal back porch
  70,     // HSYNC pulse width

  12,     // Vertical front porch
  23,     // Vertical back porch
  10      // VSYNC pulse width
};

void RA8876::writeCmd(uint8_t x)
{
  digitalWrite(m_csPin, LOW);
  SPI.transfer(RA8876_CMD_WRITE);
  SPI.transfer(x);
  digitalWrite(m_csPin, HIGH);
}

void RA8876::writeData(uint8_t x)
{
  digitalWrite(m_csPin, LOW);
  SPI.transfer(RA8876_DATA_WRITE);
  SPI.transfer(x);
  digitalWrite(m_csPin, HIGH);
}

uint8_t RA8876::readData(void)
{
  digitalWrite(m_csPin, LOW);
  SPI.transfer(RA8876_DATA_READ);
  uint8_t x = SPI.transfer(0);
  digitalWrite(m_csPin, HIGH);
  return x;
}

// Reads the special status register.
// This register uses a special cycle type instead of having an address like other registers.
// See data sheet section 19.1.
uint8_t RA8876::readStatus(void)
{
  digitalWrite(m_csPin, LOW);
  SPI.transfer(RA8876_STATUS_READ);
  uint8_t x = SPI.transfer(0);
  digitalWrite(m_csPin, HIGH);
  return x;
}

void RA8876::writeReg(uint8_t reg, uint8_t x)
{
  writeCmd(reg);
  writeData(x);
}

// Like writeReg(), but does two successive register writes of a 16-bit value, low byte first.
void RA8876::writeReg16(uint8_t reg, uint16_t x)
{
  writeCmd(reg);
  writeData(x & 0xFF);
  writeCmd(reg + 1);
  writeData(x >> 8);
}

uint8_t RA8876::readReg(uint8_t reg)
{
  writeCmd(reg);
  return readData();
}

// Like readReg(), but does two successive register reads of a 16-bit value, low byte first.
uint16_t RA8876::readReg16(uint8_t reg)
{
  uint16_t v;

  writeCmd(reg);
  v = readData();
  writeCmd(reg + 1);
  v |= readData() << 8;

  return v;
}

RA8876::RA8876(int csPin, int resetPin)
{
  m_csPin    = csPin;
  m_resetPin = resetPin;

  m_width  = 0;
  m_height = 0;
  m_depth  = 0;

  m_oscClock = 10000;  // 10000kHz or 10MHz

  m_sdramInfo = &defaultSdramInfo;

  m_displayInfo = &defaultDisplayInfo;

  m_textColor = 0xFFFF; // White

  m_fontRomInfo.present = false;  // No external font ROM chip
}

// Trigger a hardware reset.
void RA8876::hardReset(void)
{
  delay(5);
  digitalWrite(m_resetPin, LOW);
  delay(5);
  digitalWrite(m_resetPin, HIGH);
  delay(5);

  return;
}

// Trigger a soft reset. Note that the data sheet section 19.2 says that this only resets the
//  "internal state machine", not any configuration registers.
void RA8876::softReset(void)
{
  SPI.beginTransaction(m_spiSettings);

  // Trigger soft reset
  writeReg(RA8876_REG_SRR, 0x01);
  delay(5);

  // Wait for status register to show "normal operation".
  uint8_t status;
  for (int i = 0; i < 250; i++)
  {
    delay(1);

    if (((status = readStatus()) & 0x02) == 0)
      break;
  }

  SPI.endTransaction();

  return;
}

// Given a target frequency in kHz, finds PLL parameters k and n to reach as
//  close as possible to the target frequency without exceeding it.
// The k parameter will be constrained to the range 1..kMax.
// Returns true iff PLL params were found, even if not an exact match.
bool RA8876::calcPllParams(uint32_t targetFreq, int kMax, PllParams *pll)
{
  bool found = false;
  int foundk, foundn;
  uint32_t foundFreq;
  uint32_t foundError;  // Amount lower than requested frequency

  // k of 0 (i.e. 2 ** 0 = 1) is possible, but not sure if it's a good idea.
  for (int testk = 1; testk <= kMax; testk++)
  {
    if (m_oscClock % (1 << testk))
      continue;  // Step size with this k would be fractional

    int testn = (targetFreq / (m_oscClock / (1 << testk))) - 1;
    if ((testn < 1) || (testn > 63))
      continue;  // Param n out of range for this k

    // Fvco constraint found in data sheet section 6.1.2
    uint32_t fvco = m_oscClock * (testn + 1);
    if ((fvco < 100000) && (fvco > 600000))
      continue;  // Fvco out of range

    // Found some usable params, but perhaps at a lower frequency than requested.
    uint32_t freq = (m_oscClock * (testn + 1)) / (1 << testk);
    uint32_t error = targetFreq - freq;
    if ((!found) || (found && (foundError > error)))
    {
      found = true;
      foundk = testk;
      foundn = testn;
      foundFreq = freq;
      foundError = error;

      // No need to keep searching if the frequency match was exact
      if (foundError == 0)
        break;
    }
  }

  if (found)
  {
    pll->freq = foundFreq;
    pll->k    = foundk;
    pll->n    = foundn;
  }

  return found;
}

// Calculates the clock frequencies and their PLL parameters.
bool RA8876::calcClocks(void)
{
  // Data sheet section 5.2 gives max clocks:
  //  memClock : 166 MHz
  //  coreClock: 120 MHz (133MHz if not using internal font)
  //  scanClock: 100 MHz

  // Mem clock target is the same as SDRAM speed, but capped at 166 MHz
  uint32_t memClock = m_sdramInfo->speed * 1000;
  if (memClock > 166000)
    memClock = 166000;

  if (!calcPllParams(memClock, 3, &m_memPll))
    return false;

  // Core clock target will be the same as the mem clock, but capped to
  //  120 MHz, because that is the max frequency if we want to use the
  //  internal font.
  uint32_t coreClock = m_memPll.freq;
  if (coreClock > 120000)
    coreClock = 120000;

  if (!calcPllParams(coreClock, 3, &m_corePll))
    return false;

  // Scan clock target will be the display's dot clock, but capped at 100 MHz
  uint32_t scanClock = m_displayInfo->dotClock;
  if (scanClock > 100000)
    scanClock = 100000;

  if (!calcPllParams(scanClock, 7, &m_scanPll))
    return false;

  dumpClocks();

  // Data sheet section 6.1.1 rules:
  // 1. Core clock must be less than or equal to mem clock
  if (m_corePll.freq > m_memPll.freq)
    return false;

  // 2. Core clock must be greater than half mem clock
  if ((m_corePll.freq * 2) <= m_memPll.freq)
    return false;

  // 3. Core clock must be greater than (scan clock * 1.5)
  if (m_corePll.freq <= (m_scanPll.freq + (m_scanPll.freq >> 1)))
    return false;

  return true;
}

// Dump clock info to serial monitor.
void RA8876::dumpClocks(void)
{
  // Serial.println("\nMem\n---");
  // Serial.print("Requested kHz: "); Serial.println(m_sdramInfo->speed * 1000);
  // Serial.print("Actual kHz   : "); Serial.println(m_memPll.freq);
  // Serial.print("PLL k        : "); Serial.println(m_memPll.k);
  // Serial.print("PLL n        : "); Serial.println(m_memPll.n);

  // Serial.println("\nCore\n----");
  // Serial.print("kHz          : "); Serial.println(m_corePll.freq);
  // Serial.print("PLL k        : "); Serial.println(m_corePll.k);
  // Serial.print("PLL n        : "); Serial.println(m_corePll.n);

  // Serial.println("\nScan\n----");
  // Serial.print("Requested kHz: "); Serial.println(m_displayInfo->dotClock);
  // Serial.print("Actual kHz   : "); Serial.println(m_scanPll.freq);
  // Serial.print("PLL k        : "); Serial.println(m_scanPll.k);
  // Serial.print("PLL n        : "); Serial.println(m_scanPll.n);

  // TODO: Frame rate?

  return;
}

bool RA8876::initPLL(void)
{
  //  Serial.println("init PLL");

  SPI.beginTransaction(m_spiSettings);

  //Serial.print("DRAM_FREQ "); Serial.println(m_memPll.freq);
  //Serial.print("7: "); Serial.println(m_memPll.k << 1);
  //Serial.print("8: "); Serial.println(m_memPll.n);
  writeReg(RA8876_REG_MPLLC1, m_memPll.k << 1);
  writeReg(RA8876_REG_MPLLC2, m_memPll.n);

  //Serial.print("CORE_FREQ "); Serial.println(m_corePll.freq);
  //Serial.print("9: "); Serial.println(m_corePll.k << 1);
  //Serial.print("A: "); Serial.println(m_corePll.n);
  writeReg(RA8876_REG_SPLLC1, m_corePll.k << 1);
  writeReg(RA8876_REG_SPLLC2, m_corePll.n);

  // Per the data sheet, there are two divider fields for the scan clock, but the math seems
  //  to work out if we treat k as a single 3-bit number in bits 3..1.
  //Serial.print("SCAN_FREQ "); Serial.println(m_scanPll.freq);
  //Serial.print("5: "); Serial.println(m_scanPll.k << 1);
  //Serial.print("6: "); Serial.println(m_scanPll.n);
  writeReg(RA8876_REG_PPLLC1, m_scanPll.k << 1);
  writeReg(RA8876_REG_PPLLC2, m_scanPll.n);

  // Toggle bit 7 of the CCR register to trigger a reconfiguration of the PLLs
  writeReg(RA8876_REG_CCR, 0x00);
  delay(2);
  writeReg(RA8876_REG_CCR, 0x80);
  delay(2);

  uint8_t ccr = readReg(RA8876_REG_CCR);

  SPI.endTransaction();

  return (ccr & 0x80) ? true : false;
}

// Initialize SDRAM interface.
bool RA8876::initMemory(SdramInfo *info)
{
  //  Serial.println("init memory");

  uint32_t sdramRefreshRate;
  uint8_t sdrar = 0x00;
  uint8_t sdrmd = 0x00;

  // Refresh rate
  sdramRefreshRate = ((uint32_t) info->refresh * info->speed * 1000) >> info->rowBits;

  // Number of banks
  if (info->banks == 2)
    ;  // NOP
  else if (info->banks == 4)
    sdrar |= 0x20;
  else
    return false;  // Unsupported number of banks

  // Number of row bits
  if ((info->rowBits < 11) || (info->rowBits > 13))
    return false;  // Unsupported number of row bits
  else
    sdrar |= ((info->rowBits - 11) & 0x03) << 3;

  // Number of column bits
  if ((info->colBits < 8) || (info->colBits > 12))
    return false;  // Unsupported number of column bits
  else
    sdrar |= info->colBits & 0x03;

  // CAS latency
  if ((info->casLatency < 2) || (info->casLatency > 3))
    return false;  // Unsupported CAS latency
  else
    sdrmd |= info->casLatency & 0x03;

  SPI.beginTransaction(m_spiSettings);

  //  Serial.print("SDRAR: "); Serial.println(sdrar);  // Expected: 0x29 (41 decimal)
  writeReg(RA8876_REG_SDRAR, sdrar);

  //  Serial.print("SDRMD: "); Serial.println(sdrmd);
  writeReg(RA8876_REG_SDRMD, sdrmd);

  //  Serial.print("sdramRefreshRate: "); Serial.println(sdramRefreshRate);
  writeReg(RA8876_REG_SDR_REF_ITVL0, sdramRefreshRate & 0xFF);
  writeReg(RA8876_REG_SDR_REF_ITVL1, sdramRefreshRate >> 8);

  // Trigger SDRAM initialization
  writeReg(RA8876_REG_SDRCR, 0x01);

  // Wait for SDRAM to be ready
  uint8_t status;
  for (int i = 0; i < 250; i++)
  {
    delay(1);

    if ((status = readStatus()) & 0x40)
      break;
  }

  SPI.endTransaction();

  //  Serial.println(status);

  return (status & 0x40) ? true : false;
}

bool RA8876::initDisplay()
{
  SPI.beginTransaction(m_spiSettings);

  // Set chip config register
  uint8_t ccr = readReg(RA8876_REG_CCR);
  ccr &= 0xE7;  // 24-bit LCD output
  ccr &= 0xFE;  // 8-bit host data bus
  writeReg(RA8876_REG_CCR, ccr);

  writeReg(RA8876_REG_MACR, 0x00);  // Direct write, left-to-right-top-to-bottom memory

  writeReg(RA8876_REG_ICR, 0x00);  // Graphics mode, memory is SDRAM

  uint8_t dpcr = readReg(RA8876_REG_DPCR);
  dpcr &= 0xFB;  // Vertical scan top to bottom
  dpcr &= 0xF8;  // Colour order RGB
  dpcr |= 0x80;  // Panel fetches PDAT at PCLK falling edge
  writeReg(RA8876_REG_DPCR, dpcr);

  uint8_t pcsr = readReg(RA8876_REG_PCSR);
  pcsr |= 0x80;  // XHSYNC polarity high
  pcsr |= 0x40;  // XVSYNC polarity high
  pcsr &= 0xDF;  // XDE polarity high
  writeReg(RA8876_REG_PCSR, pcsr);

  // Set display width
  writeReg(RA8876_REG_HDWR, (m_displayInfo->width / 8) - 1);
  writeReg(RA8876_REG_HDWFTR, (m_displayInfo->width % 8));

  // Set display height
  writeReg(RA8876_REG_VDHR0, (m_displayInfo->height - 1) & 0xFF);
  writeReg(RA8876_REG_VDHR1, (m_displayInfo->height - 1) >> 8);

  // Set horizontal non-display (back porch)
  writeReg(RA8876_REG_HNDR, (m_displayInfo->hBackPorch / 8) - 1);
  writeReg(RA8876_REG_HNDFTR, (m_displayInfo->hBackPorch % 8));

  // Set horizontal start position (front porch)
  writeReg(RA8876_REG_HSTR, ((m_displayInfo->hFrontPorch + 4) / 8) - 1);

  // Set HSYNC pulse width
  writeReg(RA8876_REG_HPWR, ((m_displayInfo->hPulseWidth + 4) / 8) - 1);

  // Set vertical non-display (back porch)
  writeReg(RA8876_REG_VNDR0, (m_displayInfo->vBackPorch - 1) & 0xFF);
  writeReg(RA8876_REG_VNDR1, (m_displayInfo->vBackPorch - 1) >> 8);

  // Set vertical start position (front porch)
  writeReg(RA8876_REG_VSTR, m_displayInfo->vFrontPorch - 1);

  // Set VSYNC pulse width
  writeReg(RA8876_REG_VPWR, m_displayInfo->vPulseWidth - 1);

  // Set main window to 16 bits per pixel
  writeReg(RA8876_REG_MPWCTR, 0x04);  // PIP windows disabled, 16-bpp, enable sync signals

  // Set main window start address to 0
  writeReg(RA8876_REG_MISA0, 0);
  writeReg(RA8876_REG_MISA1, 0);
  writeReg(RA8876_REG_MISA2, 0);
  writeReg(RA8876_REG_MISA3, 0);

  // Set main window image width
  writeReg(RA8876_REG_MIW0, m_width & 0xFF);
  writeReg(RA8876_REG_MIW1, m_width >> 8);

  // Set main window start coordinates
  writeReg(RA8876_REG_MWULX0, 0);
  writeReg(RA8876_REG_MWULX1, 0);
  writeReg(RA8876_REG_MWULY0, 0);
  writeReg(RA8876_REG_MWULY1, 0);

  // Set canvas start address
  writeReg(RA8876_REG_CVSSA0, 0);
  writeReg(RA8876_REG_CVSSA1, 0);
  writeReg(RA8876_REG_CVSSA2, 0);
  writeReg(RA8876_REG_CVSSA3, 0);

  // Set canvas width
  writeReg(RA8876_REG_CVS_IMWTH0, m_width & 0xFF);
  writeReg(RA8876_REG_CVS_IMWTH1, m_width >> 8);

  // Set active window start coordinates
  writeReg(RA8876_REG_AWUL_X0, 0);
  writeReg(RA8876_REG_AWUL_X1, 0);
  writeReg(RA8876_REG_AWUL_Y0, 0);
  writeReg(RA8876_REG_AWUL_Y1, 0);

  // Set active window dimensions
  writeReg(RA8876_REG_AW_WTH0, m_width & 0xFF);
  writeReg(RA8876_REG_AW_WTH1, m_width >> 8);
  writeReg(RA8876_REG_AW_HT0, m_height & 0xFF);
  writeReg(RA8876_REG_AW_HT1, m_height >> 8);

  // Set canvas addressing mode/colour depth
  uint8_t aw_color = 0x00;  // 2d addressing mode
  if (m_depth == 16)
    aw_color |= 0x01;
  else if (m_depth == 24)
    aw_color |= 0x02;
  writeReg(RA8876_REG_AW_COLOR, aw_color);

  // Turn on display
  dpcr = readReg(RA8876_REG_DPCR);
  dpcr |= 0x40;  // Display on
  writeReg(RA8876_REG_DPCR, dpcr);

  // TODO: Track backlight pin and turn on backlight

  SPI.endTransaction();

  return true;
}

bool RA8876::init(void)
{
  m_width  = m_displayInfo->width;
  m_height = m_displayInfo->height;
  m_depth  = 16;

  // Set up chip select pin
  pinMode(m_csPin, OUTPUT);
  digitalWrite(m_csPin, HIGH);

  // Set up reset pin, if provided
  if (m_resetPin >= 0)
  {
    pinMode(m_resetPin, OUTPUT);
    digitalWrite(m_resetPin, HIGH);

    hardReset();
  }

  if (!calcClocks())
  {
    //    Serial.println("calcClocks failed");
    return false;
  }

  SPI.begin();
  
  m_spiSettings = SPISettings(RA8876_SPI_SPEED, MSBFIRST, SPI_MODE3);

  // SPI is now up, so we can do a soft reset if no hard reset was possible earlier
  if (m_resetPin < 0)
    softReset();

  if (!initPLL())
  {
    //    Serial.println("initPLL failed");
    return false;
  }

  if (!initMemory(m_sdramInfo))
  {
    //    Serial.println("initMemory failed");
    return false;
  }

  if (!initDisplay())
  {
    //    Serial.println("initDisplay failed");
    return false;
  }

  // Set default font
//  selectInternalFont(RA8876_FONT_SIZE_16);
//  setTextScale(1);
  
// GEVA  
  clearScreen(0);
  PWM0(1, 3, 100, 100, 100);     // 0 to 100% Backlight
  FontCanvasLoad();                     // Copio i font da Flash a RAM del Controller
//  SetFont(&notoSans_8pt);
  setTextColor(0xFFFFFF);  
  
  writeReg(RA8876_REG_SPI_DIVSOR, 0);

  Select_SFI_Dual_Mode_Dummy_8T_3Bh();
//void Select_SFI_Dual_Mode_Dummy_4T_BBh(void);

  // If an SD card with font.bin / All_76_Pic_65K.bin is inserted, (re)load them
  // into the display serial flash. Does nothing when no SD / no such files.
  copySDtoFlash();

  return true;
}

// =============================================================================
// SD -> serial-flash auto-loader (was CopySDonFlash/WriteFlash in
// gevino_tft_display.h; moved into the class so init() runs it on every boot).
// =============================================================================
unsigned char RA8876::flashReadStatus(void) {   // Serial flash Read Status Register (0x05)
  unsigned char temp;
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x05);
  // No delay here: SPI_Master_FIFO_Data_Put() already waits for the byte to be
  // shifted out and reads the incoming one back. A delay(1) in this function
  // costs a full millisecond on every busy poll, i.e. minutes over a whole
  // flash image.
  temp = SPI_Master_FIFO_Data_Put(0xff);        // dummy cycle, read back value
  nSS_Inactive();
  return temp;
}

// =============================================================================
// Bulk transfer to the serial flash through the RA8876 SPI master.
//
// SPI_Master_FIFO_Data_Put() spends 8 separate SPI transactions with the
// controller on every single byte: the byte itself, two FIFO polls, and the
// read back of the incoming byte, which a page program does not need. That is
// ~30us and 16 chip-select frames per useful byte, about 33 KB/s, while the
// flash side is clocked at 30 MHz and would take 3.7 MB/s.
//
// Here the data port REG[B8h] is selected once and the bytes are shifted in a
// single chip-select frame, which the RAiO SPI protocol allows: leading byte
// 0x80, then as many data bytes as wanted. The master FIFO is 16 bytes deep,
// so one frame carries at most 16 bytes; before the next frame the TX FIFO
// must have drained and the RX FIFO must be emptied, or the master stops.
//
// Returns false if a FIFO does not move within the timeout, so the caller can
// fall back to the byte-by-byte path instead of hanging.
// =============================================================================
// The handshake every 16 bytes is what keeps this safe, and it costs about
// half the time. The flash side is clocked at 30 MHz and the host at 12, so
// in practice the FIFO never fills and a whole page could go in one frame:
// raising RA8876_SPIM_FIFO to 256 roughly doubles the speed again. Only do
// that with a verified image in hand - if the assumption is ever wrong the
// bytes are lost silently, with no timeout to catch it.
#define RA8876_SPIM_FIFO  16        // SPI master FIFO depth, in bytes
#define RA8876_SPIM_WAIT  5000      // us to wait for a FIFO before giving up

bool RA8876::spiMasterBurst(const uint8_t *buf, uint16_t len)
{
  uint32_t t0;

  while (len) {
    uint16_t n = (len > RA8876_SPIM_FIFO) ? RA8876_SPIM_FIFO : len;

    t0 = micros();                            // room for the whole chunk
    while (!Tx_FIFO_Empty_Flag())
      if (micros() - t0 > RA8876_SPIM_WAIT) return false;

    writeCmd(0xB8);                           // FIFO data port
    digitalWrite(m_csPin, LOW);
    SPI.transfer(RA8876_DATA_WRITE);
    for (uint16_t i = 0; i < n; i++) SPI.transfer(buf[i]);
    digitalWrite(m_csPin, HIGH);

    t0 = micros();                            // last byte out of the shifter
    while (!Tx_FIFO_Empty_Flag())
      if (micros() - t0 > RA8876_SPIM_WAIT) return false;

    writeCmd(0xB8);                           // drop what came back
    digitalWrite(m_csPin, LOW);
    SPI.transfer(RA8876_DATA_READ);
    for (uint16_t i = 0; i < n; i++) SPI.transfer(0);
    digitalWrite(m_csPin, HIGH);

    t0 = micros();                            // in case it pops one at a time
    while (!Rx_FIFO_Empty_Flag()) {
      SPI_Master_FIFO_Data_Get();
      if (micros() - t0 > RA8876_SPIM_WAIT) return false;
    }

    buf += n;
    len -= n;
  }
  return true;
}

void RA8876::flashWriteEnable(void) {           // WREN (06h), until WEL is set
  do {
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x06);
    nSS_Inactive();
  } while (!(flashReadStatus() & 0x02));
}

void RA8876::flashEraseBlock(unsigned long addr) {   // 64 KB block erase (D8h)
  flashWriteEnable();
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0xD8);
#ifdef Flash_256Mb
  SPI_Master_FIFO_Data_Put(addr >> 24);
#endif
  SPI_Master_FIFO_Data_Put(addr >> 16);
  SPI_Master_FIFO_Data_Put(addr >> 8);
  SPI_Master_FIFO_Data_Put(addr);
  nSS_Inactive();
  while (flashReadStatus() & 0x01);
}

// One page (up to 256 bytes, never across a page boundary). false means the
// burst gave up half way: the page holds whatever arrived before that, and the
// caller is expected to program it again byte by byte. Writing the same page
// twice is harmless, programming only clears bits that are already clear.
bool RA8876::flashPageProgram(unsigned long addr, const uint8_t *src,
                              uint16_t n, bool burst) {
  bool ok = true;
  flashWriteEnable();
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x02);               // page program
#ifdef Flash_256Mb
  SPI_Master_FIFO_Data_Put(addr >> 24);
#endif
  SPI_Master_FIFO_Data_Put(addr >> 16);
  SPI_Master_FIFO_Data_Put(addr >> 8);
  SPI_Master_FIFO_Data_Put(addr);
  if (burst) ok = spiMasterBurst(src, n);
  else for (uint16_t i = 0; i < n; i++) SPI_Master_FIFO_Data_Put(src[i]);
  nSS_Inactive();
  while (flashReadStatus() & 0x01);
  return ok;
}

void RA8876::copySDtoFlash(void) {
  SdFat sd;
  if (!sd.begin(12, SD_SCK_MHZ(12))) return;     // no card / not ready -> nothing to do
  selectInternalFont(RA8876_FONT_SIZE_16);       // valid text mode for the writeFlash() progress prints
  bool wroteFont = false;
  // Each call erases only the blocks it writes, so the two areas do not
  // disturb each other and neither one pays for a chip erase.
  if (sd.exists("font.bin")) {
    writeFlash(0, "font.bin", 1);
    wroteFont = true;
  }
  if (sd.exists("All_76_Pic_65K.bin")) {
    writeFlash(Pic_Offset, "All_76_Pic_65K.bin", 1);
  }
  if (wroteFont) FontCanvasLoad();               // reload fonts to canvas RAM (NOT a full re-init)
}

// erase: 0 = write on what is already there, anything else = erase each 64 KB
// block just before writing into it. Only the blocks actually written are
// erased, so what lies beyond the end of the file is left alone (nothing
// points at it) and no time is spent erasing it. A chip erase on a W25Q128 is
// 40 s typical and up to 200; the ~60 blocks of a font image are 9.
//
// If flash_addr is not on a 64 KB boundary the block that contains it is
// erased all the same, so whatever shares that block is lost. Both callers
// are aligned: fonts start at 0, images at Pic_Offset.
void RA8876::writeFlash(unsigned long flash_addr, const char *file, byte erase) {
  unsigned long TimeToWrite = millis();
  unsigned long nextBlock = flash_addr & ~0xFFFFUL;  // first block to erase
  int wDiv = 0;
  bool burst = true;                 // until the controller FIFO says otherwise
  uint8_t buffer[512];               // a whole SD sector per read
  SdFile ff;
  if (!ff.open(file, FILE_READ)) return;
  Enable_SFlash_SPI();
#ifdef WriteC_0
  Select_nSS_drive_on_xnsfcs0();
#else
  Select_nSS_drive_on_xnsfcs1();
#endif
  SPI_Clock_Period(1);
#ifdef Flash_256Mb
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0xB7);                // enter 4-byte address mode
  delay(1);
  nSS_Inactive();
#endif
  print("  Flash Write ");

  // SD -> SAMD -> RA8876 -> flash, erasing each block on the way in
  for (;;) {
    int got = ff.read(buffer, sizeof(buffer));
    if (got <= 0) break;

    for (int off = 0; off < got; off += 256) {
      uint16_t n = (got - off > 256) ? 256 : (uint16_t)(got - off);

      if (erase && flash_addr >= nextBlock) {    // the block we are entering
        flashEraseBlock(nextBlock);
        nextBlock += 0x10000;
        print("E");
      }

      if (!flashPageProgram(flash_addr, buffer + off, n, burst)) {
        burst = false;                           // FIFO stuck: slow path, and
        flashPageProgram(flash_addr, buffer + off, n, false);   // same page
        print("!");
      }
      flash_addr += n;
      if (++wDiv >= 400) { print("w"); wDiv = 0; }
    }
  }

  nSS_Active(); SPI_Master_FIFO_Data_Put(0x04); nSS_Inactive();   // write disable
  while (flashReadStatus() & 0x01);
  flashReadStatus();
  Check_2D_Busy();
  nSS_Active(); SPI_Master_FIFO_Data_Put(0xE9); delay(1); nSS_Inactive();   // exit 4-byte mode
  delay(10);
  Disable_SFlash_SPI();
  ff.close();
  print(" on ");
  print((millis() - TimeToWrite) / 1000);
  print(" Sec");
}

void RA8876::initExternalFontRom(int spiIf, enum ExternalFontRom chip)
{
  // See data sheet figure 16-10
  // TODO: GT30L24T3Y supports FAST_READ command (0x0B) and runs at 20MHz. Are the other font chips the same?

  // Find a clock divisor. Values are in the range 2..512 in steps of 2.
  int divisor;
  uint32_t speed = 20000;  // 20MHz target speed
  for (divisor = 2; divisor <= 512; divisor += 2)
  {
    if (m_corePll.freq / divisor <= speed)
      break;
  }

  m_fontRomInfo.present = true;
  m_fontRomInfo.spiInterface = spiIf;
  m_fontRomInfo.spiClockDivisor = divisor;
  m_fontRomInfo.chip = chip;

  //  Serial.print("External font SPI divisor: "); Serial.println(divisor);

  SPI.beginTransaction(m_spiSettings);

  // Ensure SPI is enabled in chip config register
  uint8_t ccr = readReg(RA8876_REG_CCR);
  if (!(ccr & 0x02))
    writeReg(RA8876_REG_CCR, ccr | 0x02);

  //  Serial.print("SFL_CTRL: "); Serial.println(((spiIf & 1) << 7) | 0x14, HEX);
  writeReg(RA8876_REG_SFL_CTRL, ((spiIf & 1) << 7) | 0x14);  // Font mode, 24-bit address, standard timing, supports FAST_READ
  //  Serial.print("SPI_DIVSOR: "); Serial.println((divisor >> 1) - 1, HEX);
  writeReg(RA8876_REG_SPI_DIVSOR, (divisor >> 1) - 1);
  //  Serial.print("GTFNT_SEL: "); Serial.println((chip & 0x07) << 5, HEX);
  writeReg(RA8876_REG_GTFNT_SEL, (chip & 0x07) << 5);

  SPI.endTransaction();
}

// Show colour bars of 8 colours in repeating horizontal bars.
// This does not alter video memory, but rather instructs the video controller to display
//  the pattern rather than the contents of memory.
void RA8876::colorBarTest(bool enabled)
{
  SPI.beginTransaction(m_spiSettings);

  uint8_t dpcr = readReg(RA8876_REG_DPCR);

  if (enabled)
    dpcr = dpcr | 0x20;
  else
    dpcr = dpcr & ~0x20;

  writeReg(RA8876_REG_DPCR, dpcr);

  SPI.endTransaction();
}

void RA8876::drawPixel(int x, int y, uint16_t color)
{
  //Serial.println("drawPixel");
  //Serial.println(readStatus());

  SPI.beginTransaction(m_spiSettings);

  writeReg(RA8876_REG_CURH0, x & 0xFF);
  writeReg(RA8876_REG_CURH1, x >> 8);

  writeReg(RA8876_REG_CURV0, y & 0xFF);
  writeReg(RA8876_REG_CURV1, y >> 8);

  writeReg(RA8876_REG_MRWDP, color & 0xFF);
  writeReg(RA8876_REG_MRWDP, color >> 8);

  SPI.endTransaction();
}

void RA8876::drawTwoPointShape(int x1, int y1, int x2, int y2, uint16_t color, uint8_t reg, uint8_t cmd)
{
  //Serial.println("drawTwoPointShape");

  SPI.beginTransaction(m_spiSettings);

  // First point
  writeReg(RA8876_REG_DLHSR0, x1 & 0xFF);
  writeReg(RA8876_REG_DLHSR1, x1 >> 8);
  writeReg(RA8876_REG_DLVSR0, y1 & 0xFF);
  writeReg(RA8876_REG_DLVSR1, y1 >> 8);

  // Second point
  writeReg(RA8876_REG_DLHER0, x2 & 0xFF);
  writeReg(RA8876_REG_DLHER1, x2 >> 8);
  writeReg(RA8876_REG_DLVER0, y2 & 0xFF);
  writeReg(RA8876_REG_DLVER1, y2 >> 8);

  // Colour
  writeReg(RA8876_REG_FGCR, color >> 11 << 3);
  writeReg(RA8876_REG_FGCG, ((color >> 5) & 0x3F) << 2);
  writeReg(RA8876_REG_FGCB, (color & 0x1F) << 3);

  // Draw
  writeReg(reg, cmd);  // Start drawing

  // Wait for completion
  uint8_t status = readStatus();
  int iter = 0;
  while (status & 0x08)
  {
    status = readStatus();
    iter++;
  }

  //Serial.print(iter); Serial.println(" iterations");

  SPI.endTransaction();
}

void RA8876::drawThreePointShape(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color, uint8_t reg, uint8_t cmd)
{
  //Serial.println("drawThreePointShape");

  SPI.beginTransaction(m_spiSettings);

  // First point
  writeReg(RA8876_REG_DLHSR0, x1 & 0xFF);
  writeReg(RA8876_REG_DLHSR1, x1 >> 8);
  writeReg(RA8876_REG_DLVSR0, y1 & 0xFF);
  writeReg(RA8876_REG_DLVSR1, y1 >> 8);

  // Second point
  writeReg(RA8876_REG_DLHER0, x2 & 0xFF);
  writeReg(RA8876_REG_DLHER1, x2 >> 8);
  writeReg(RA8876_REG_DLVER0, y2 & 0xFF);
  writeReg(RA8876_REG_DLVER1, y2 >> 8);

  // Third point
  writeReg(RA8876_REG_DTPH0, x3 & 0xFF);
  writeReg(RA8876_REG_DTPH1, x3 >> 8);
  writeReg(RA8876_REG_DTPV0, y3 & 0xFF);
  writeReg(RA8876_REG_DTPV1, y3 >> 8);

  // Colour
  writeReg(RA8876_REG_FGCR, color >> 11 << 3);
  writeReg(RA8876_REG_FGCG, ((color >> 5) & 0x3F) << 2);
  writeReg(RA8876_REG_FGCB, (color & 0x1F) << 3);

  // Draw
  writeReg(reg, cmd);  // Start drawing

  // Wait for completion
  uint8_t status = readStatus();
  int iter = 0;
  while (status & 0x08)
  {
    status = readStatus();
    iter++;
  }

  //Serial.print(iter); Serial.println(" iterations");

  SPI.endTransaction();
}

void RA8876::drawEllipseShape(int x, int y, int xrad, int yrad, uint16_t color, uint8_t cmd)
{
  //Serial.println("drawEllipseShape");

  SPI.beginTransaction(m_spiSettings);

  // First point
  writeReg16(RA8876_REG_DEHR0, x);
  writeReg16(RA8876_REG_DEVR0, y);

  // Radii
  writeReg16(RA8876_REG_ELL_A0, xrad);
  writeReg16(RA8876_REG_ELL_B0, yrad);

  // Colour
  writeReg(RA8876_REG_FGCR, color >> 11 << 3);
  writeReg(RA8876_REG_FGCG, ((color >> 5) & 0x3F) << 2);
  writeReg(RA8876_REG_FGCB, (color & 0x1F) << 3);

  // Draw
  writeReg(RA8876_REG_DCR1, cmd);  // Start drawing

  // Wait for completion
  uint8_t status = readStatus();
  int iter = 0;
  while (status & 0x08)
  {
    status = readStatus();
    iter++;
  }

  //Serial.print(iter); Serial.println(" iterations");

  SPI.endTransaction();
}
void RA8876::setCursor(int x, int y)
{
  // Update the local coordinates (for use by custom font mode)
  // as well as the RA8876 (used by internal ROM font)
	
  X_Cursor = x;
  Y_Cursor = y;

  SPI.beginTransaction(m_spiSettings);

  writeReg16(RA8876_REG_F_CURX0, x);
  writeReg16(RA8876_REG_F_CURY0, y);

  SPI.endTransaction();
}

int RA8876::getCursorX(void)
{
  // NOTE: We could potentially always return X_Cursor, but
  // the following was done to increase compatibility
  if( m_fontSource == RA8876_FONT_SOURCE_EXT_GEVA ){
	return X_Cursor;
  } else {	
    SPI.beginTransaction(m_spiSettings);

    int x = readReg16(RA8876_REG_F_CURX0);

    SPI.endTransaction();

    return x;
  }
}

int RA8876::getCursorY(void)
{
  // NOTE: We could potentially always return Y_Cursor, but
  // the following was done to increase compatibility
  if( m_fontSource == RA8876_FONT_SOURCE_EXT_GEVA ){
	return Y_Cursor;
  } else {		
    SPI.beginTransaction(m_spiSettings);

    int y = readReg16(RA8876_REG_F_CURY0);

    SPI.endTransaction();

    return y;
  }
}
// Given a font encoding value, returns the corresponding bit pattern for
//  use by internal fonts.
uint8_t RA8876::internalFontEncoding(enum FontEncoding enc)
{
  uint8_t e;
  switch (enc)
  {
    case RA8876_FONT_ENCODING_8859_2:
      e = 0x01;
      break;
    case RA8876_FONT_ENCODING_8859_4:
      e = 0x02;
      break;
    case RA8876_FONT_ENCODING_8859_5:
      e = 0x03;
      break;
    default:
      e = 0x00;  // ISO-8859-1
      break;
  }

  return e;
}

void RA8876::setTextMode(void)
{
  // Restore text colour
  writeReg(RA8876_REG_FGCR, m_textColor >> 11 << 3);
  writeReg(RA8876_REG_FGCG, ((m_textColor >> 5) & 0x3F) << 2);
  writeReg(RA8876_REG_FGCB, (m_textColor & 0x1F) << 3);

  waitTaskBusy();

  // Enable text mode
  uint8_t icr = readReg(RA8876_REG_ICR);
  writeReg(RA8876_REG_ICR, icr | 0x04);
}

void RA8876::setGraphicsMode(void)
{
  waitTaskBusy();

  // Disable text mode
  uint8_t icr = readReg(RA8876_REG_ICR);
  writeReg(RA8876_REG_ICR, icr & ~0x04);
}

void RA8876::selectInternalFont(enum FontSize size, enum FontEncoding enc)
{
  m_fontSource = RA8876_FONT_SOURCE_INTERNAL;
  m_fontSize   = size;
  m_fontFlags  = 0;

  SPI.beginTransaction(m_spiSettings);

  writeReg(RA8876_REG_CCR0, 0x00 | ((size & 0x03) << 4) | internalFontEncoding(enc));

  uint8_t ccr1 = readReg(RA8876_REG_CCR1);
  ccr1 |= 0x40;  // Transparent background
  writeReg(RA8876_REG_CCR1, ccr1);

  SPI.endTransaction();
}

void RA8876::selectExternalFont(enum ExternalFontFamily family, enum FontSize size, enum FontEncoding enc, FontFlags flags)
{
  m_fontSource = RA8876_FONT_SOURCE_EXT_ROM;
  m_fontSize   = size;
  m_fontFlags  = flags;

  SPI.beginTransaction(m_spiSettings);

  //  Serial.print("CCR0: "); Serial.println(0x40 | ((size & 0x03) << 4), HEX);
  writeReg(RA8876_REG_CCR0, 0x40 | ((size & 0x03) << 4));  // Select external font ROM and size

  uint8_t ccr1 = readReg(RA8876_REG_CCR1);
  ccr1 |= 0x40;  // Transparent background
  //  Serial.print("CCR1: "); Serial.println(ccr1, HEX);
  writeReg(RA8876_REG_CCR1, ccr1);

  //  Serial.print("GTFNT_CR: "); Serial.println((enc << 3) | (family & 0x03), HEX);
  writeReg(RA8876_REG_GTFNT_CR, (enc << 3) | (family & 0x03));  // Character encoding and family

  SPI.endTransaction();
}

int RA8876::getTextSizeY(void)
{
  return ((m_fontSize + 2) * 8) * m_textScaleY;
}

void RA8876::setTextScale(int xScale, int yScale)
{
  xScale = constrain(xScale, 1, 4);
  yScale = constrain(yScale, 1, 4);

  m_textScaleX = xScale;
  m_textScaleY = yScale;

  SPI.beginTransaction(m_spiSettings);

  uint8_t ccr1 = readReg(RA8876_REG_CCR1);
  ccr1 = (ccr1 & 0xF0) | ((xScale - 1) << 2) | (yScale - 1);
  //  Serial.println(ccr1, HEX);
  writeReg(RA8876_REG_CCR1, ccr1);

  SPI.endTransaction();
}

//// Similar to write(), but does no special handling of control characters.
//void RA8876::putChars(const char *buffer, size_t size)
//{
//  SPI.beginTransaction(m_spiSettings);
//
//  setTextMode();
//
//  // Write characters
//  writeCmd(RA8876_REG_MRWDP);
//  for (unsigned int i = 0; i < size; i++)
//  {
//    waitWriteFifo();
//    writeData(buffer[i]);
//  }
//
//  setGraphicsMode();
//
//  SPI.endTransaction();
//}
//
//void RA8876::putChars16(const uint16_t *buffer, unsigned int count)
//{
//  SPI.beginTransaction(m_spiSettings);
//
//  setTextMode();
//
//  // Write characters
//  writeCmd(RA8876_REG_MRWDP);
//  for (unsigned int i = 0; i < count; i++)
//  {
//    waitWriteFifo();
//    writeData(buffer[i] >> 8);
//
//    waitWriteFifo();
//    writeData(buffer[i] & 0xFF);
//  }
//
//  setGraphicsMode();
//
//  SPI.endTransaction();
//}
//
//---------- write -------------------------------------------------------
size_t RA8876::write(const uint8_t *buffer, size_t size)
{
	if( m_fontSource == RA8876_FONT_SOURCE_EXT_GEVA ){
	  for (unsigned int i = 0; i < size; i++)
	  {
		uint8_t c = buffer[i];
		if (c == '\r'){
		  X_Cursor = 0;
		  p = 0;  // previews Caracter
		}else if (c == '\n')    {
		  X_Cursor = 0;
		  int CH = Y_Cursor + FontNow->CharacterHeight;
		  if (CH < m_height)  Y_Cursor = CH;
		  p = 0;  // previews Caracter
		}else{
		  putChar(c);
		}
	  }
	  return size;
	}else{
		SPI.beginTransaction(m_spiSettings);
		setTextMode();
		writeCmd(RA8876_REG_MRWDP);  // Set current register for writing to memory
		for (unsigned int i = 0; i < size; i++){
			char c = buffer[i];
			if (c == '\r');  // Ignored
			else if (c == '\n'){
			  setCursor(0, getCursorY() + getTextSizeY());
			  writeCmd(RA8876_REG_MRWDP);  // Reset current register for writing to memory
			}else if ((m_fontFlags & RA8876_FONT_FLAG_XLAT_FULLWIDTH) && ((c >= 0x21) || (c <= 0x7F))){
			  // Translate ASCII to Unicode fullwidth form (for Chinese fonts that lack ASCII)
			  uint16_t fwc = c - 0x21 + 0xFF01;
			  waitWriteFifo();
			  writeData(fwc >> 8);
			  waitWriteFifo();
			  writeData(fwc & 0xFF);
			}else{
			  waitWriteFifo();
			  writeData(c);
			}
		}
		setGraphicsMode();
		SPI.endTransaction();
		return size;				
	}
}
//---------- getTextBounds -------------------------------------------------------
void RA8876::getTextBounds(const char *buffer, int16_t x, int16_t y,
                                 int16_t *x1, int16_t *y1, uint16_t *w,
                                 uint16_t *h) {

  if ( m_fontSource == RA8876_FONT_SOURCE_INTERNAL ){
	// We will make the assumption that only RA8876_FONT_SIZE_16
	// is being used, along with simple text scaling.
    *x1 = x;
	*y1 = y;
	*w = strlen(buffer) * m_textScaleX*8;
    *h =                  m_textScaleY*16;
  } else {
    uint8_t c, i  =0; // Current character

    *x1 = x;
    *y1 = y;
    *w = 0;
    *h = FontNow->CharacterHeight;
  
    c = buffer[i++];
    while( c != 0 && c != '/r' && c != '/n'  ){
      charBounds(c, w);
      c = buffer[i++];
    }
    *w -= FontNow->Map[('o'-' ')].Charwidth/4;
  }
}
//---------- charBounds -------------------------------------------------------
void RA8876::charBounds( uint8_t c, uint16_t *w){
  uint8_t CharSpacing = FontNow->Map[('o'-' ')].Charwidth/4;
  
  if( ((p=='A' || p=='L') && (c=='V' || c=='Y' || c=='W')) ||  \
      ((p=='P' || p=='Y' || p=='V' || p=='W') && c=='A' )) \
*w -= CharSpacing;

	c = c - ' ';
	if ( c >  100) c -= 33;   // accented letters
	int16_t with;
	if( c ) with = FontNow->Map[c].Charwidth;
	else with = FontNow->Map['i'-' '].Charwidth;    // For space, use 'i' width
	*w += with;
	*w += CharSpacing;	
}
//---------- putChar -------------------------------------------------------
//	Show image
		  // setTextColor(BLUE);  // Clear Background
		  // BTE_S0_Color_8bpp();
		  // BTE_S0_Memory_Start_Address(OffsetBuff);
		  // BTE_S0_Image_Width(8);
		  // BTE_S0_Window_Start_XY(0, 0); // X, Y

		  // BTE_Destination_Color_16bpp();
		  // BTE_Destination_Memory_Start_Address(0);
		  // BTE_Destination_Image_Width(1024);
		  // BTE_Destination_Window_Start_XY(0, 30);
		  
		  // BTE_Window_Size(48,500);

		  // BTE_Enable();
		  // delay(100);
		  
#ifdef VERTICAL
void RA8876::putChar( uint8_t c )
{
  /*
    0 space
    94  ~
    à   195 +  160 - space
    è   195 +  168 - space
    ì   195 +  172 - space
    ò   195 +  178 - space
    ù   195 +  185 - space
  */
  uint8_t CharSpacing = FontNow->Map[('o'-' ')].Charwidth/4;

  if( ((p=='A' || p=='L') && (c=='V' || c=='Y' || c=='W')) ||  \
	  ((p=='P' || p=='Y' || p=='V' || p=='W') && c=='A' )) \
	X_Cursor += CharSpacing;  //### was -
  p = c;
  
  if ( c < 195 ) {
	c = c - ' ';
	if ( c >  100) c -= 33;   // accented letters
	int16_t with;
	if( c ) with = FontNow->Map[c].Charwidth;
	else with = FontNow->Map['i'-' '].Charwidth;    // For space, use 'o' width
    X_Cursor -= with;

	if ( ( X_Cursor - with ) < m_height && ( FontNow->CharacterHeight + Y_Cursor ) < m_width ){  //### swapped
	  if(c || !ClearBG){
		if(c){
		  BTE_S0_Window_Start_XY(0, FontNow->Map[c].Offset); // X, Y ###swapped
		  BTE_Destination_Window_Start_XY(Y_Cursor, X_Cursor); // ### swapped
		  BTE_Window_Size(FontNow->CharacterHeight, with);    //### swapped
		  BTE_Enable();
		}else{
		  fillRect(X_Cursor, Y_Cursor, X_Cursor-with, Y_Cursor-FontNow->CharacterHeight, m_BGtextColor);
		  Foreground_RGB((unsigned char)(m_textColor >> 11 << 3), (unsigned char)(((m_textColor >> 5) & 0x3F) << 2),(unsigned char)((m_textColor & 0x1F) << 3));
		}
	  }
	  Check_BTE_Busy();
	  if ( ( X_Cursor - CharSpacing ) < m_height ){    	//### changed 
		if( !ClearBG ){
			fillRect(X_Cursor, Y_Cursor, X_Cursor-CharSpacing, Y_Cursor+FontNow->CharacterHeight, m_BGtextColor);
			Foreground_RGB((unsigned char)(m_textColor >> 11 << 3), (unsigned char)(((m_textColor >> 5) & 0x3F) << 2),(unsigned char)((m_textColor & 0x1F) << 3));
		}
		X_Cursor -= CharSpacing;
	  }
	}
  }
}
#else
void RA8876::putChar( uint8_t c )
{
  /*
    0 space
    94  ~
    à   195 +  160 - space
    è   195 +  168 - space
    ì   195 +  172 - space
    ò   195 +  178 - space
    ù   195 +  185 - space
  */
  uint8_t CharSpacing = FontNow->Map[('o'-' ')].Charwidth/4;
  
  if( ((p=='A' || p=='L') && (c=='V' || c=='Y' || c=='W')) ||  \
	  ((p=='P' || p=='Y' || p=='V' || p=='W') && c=='A' )) \
	X_Cursor -= CharSpacing;
  p = c;

  if ( c < 195 ) {
	c = c - ' ';
	if ( c >  100) c -= 33;   // accented letters
	int16_t with;
	if( c ) with = FontNow->Map[c].Charwidth;
	else with = FontNow->Map['i'-' '].Charwidth;    // For space, use 'o' width

	if ( ( with + X_Cursor ) < m_width && ( FontNow->CharacterHeight + Y_Cursor ) < m_height ){
	  if(c || !ClearBG){
		if(c){
		  BTE_S0_Window_Start_XY(FontNow->Map[c].Offset, 0); // X, Y
		  BTE_Destination_Window_Start_XY(X_Cursor, Y_Cursor);
		  BTE_Window_Size(with,FontNow->CharacterHeight);    
		  BTE_Enable();
		}else{
		  fillRect(X_Cursor, Y_Cursor, X_Cursor+with, Y_Cursor+FontNow->CharacterHeight, m_BGtextColor);
		  Foreground_RGB((unsigned char)(m_textColor >> 11 << 3), (unsigned char)(((m_textColor >> 5) & 0x3F) << 2),(unsigned char)((m_textColor & 0x1F) << 3));
		}
	  }
	  X_Cursor += with;
	  Check_BTE_Busy();
	  if ( ( CharSpacing + X_Cursor ) < m_width ){     
		if( !ClearBG ){
		  fillRect(X_Cursor, Y_Cursor, X_Cursor+CharSpacing, Y_Cursor+FontNow->CharacterHeight, m_BGtextColor);
		  Foreground_RGB((unsigned char)(m_textColor >> 11 << 3), (unsigned char)(((m_textColor >> 5) & 0x3F) << 2),(unsigned char)((m_textColor & 0x1F) << 3));
		}
		X_Cursor += CharSpacing;
	  }
	}
  }
}
#endif	
//---------- FontCanvasLoad -------------------------------------------------------
void RA8876::FontCanvasLoad( void ) {

  //(change to 8bpp)
  Host_Bus_8bit();
  Data_Format_8b_8bpp();
  Memory_8bpp_Mode();
  Select_Main_Window_8bpp();

//  //(DMA)
  Enable_SFlash_SPI();
  Select_SFI_0();
  Select_nSS_drive_on_xnsfcs0();
//  SPI_Clock_Period(2);

//  Canvas_Image_Start_address(OffsetBuff + FontNow->FlashStart);
//  Canvas_image_width(FontNow->BlockWidth);
//  DMA_24bit (
//    0,                            //unsigned char SCS //SCS : 0 = Use SCS0, 1 = Use SCS1
//    4,                            //,unsigned char Clk //Clk : SPI Clock = System Clock /{(Clk+1)*2}
//    0,                            //,unsigned short X1 //X of DMA Coordinate
//    0,                            //,unsigned short Y1 //Y of DMA Coordinate
//    FontNow->BlockWidth,          //,unsigned short X_W //DMA Block width
//    FontNow->CharacterHeight,     //,unsigned short Y_H //DMA Block height
//    FontNow->BlockWidth,          //,unsigned short P_W //DMA Picture width
//    FontNow->FlashStart           //,unsigned long Addr //DMA Source Start address
//  );

    Canvas_Image_Start_address(OffsetBuff);
    Canvas_image_width(0x1000);
    DMA_24bit  (
      FontChipCS,    //unsigned char SCS //SCS : 0 = Use SCS0, 1 = Use SCS1
      0,    //,unsigned char Clk //Clk : SPI Clock = System Clock /{(Clk+1)*2}
//    clk 0 = 328 mS  2.5 Mb
//    clk 1 = 728 mS
//    clk 10 = 3940 mS
      0,    //,unsigned short X1 //X of DMA Coordinate
      0,    //,unsigned short Y1 //Y of DMA Coordinate
      0x1000,   //,unsigned short X_W //DMA Block width
      ENDFLASH/0x1000+1,   //,unsigned short Y_H //DMA Block height
      0x1000,       //,unsigned short P_W //DMA Picture width
      0  //,unsigned long Addr //DMA Source Start address
    );

  Check_Busy_SFI_DMA();

  Host_Bus_8bit();
  Data_Format_8b_16bpp();
  Memory_16bpp_Mode();
  Select_Main_Window_16bpp();
  BTE_Destination_Color_16bpp();

  //  BTE_Operation_Code(0xF);    //BTE Operation: Memory write with Color Expansion and chroma keying (w/o ROP)
  BTE_Operation_Code(0xE);    //BTE Operation: Memory write with Color Expansion and chroma keying (w/o ROP)
  BTE_ROP_Code(7);  //MCU data bus width: 16bit, ROP Code=7
  BTE_S0_Color_8bpp();
//  BTE_S0_Memory_Start_Address(OffsetBuff + FontNow->FlashStart);
//  BTE_S0_Image_Width(FontNow->BlockWidth);
  BTE_Destination_Image_Width(1024);
  BTE_Destination_Memory_Start_Address(0x00000000);

  Canvas_Image_Start_address(0);
  Canvas_image_width(1024);
}
//---------- setTextColor -------------------------------------------------------
void RA8876::setTextColor(uint16_t color) {   // BackGround Clear
  m_textColor = color;
  ClearBG = 1;  
  Check_BTE_Busy();
  Foreground_RGB((unsigned char)(color >> 11 << 3), (unsigned char)(((color >> 5) & 0x3F) << 2), (unsigned char)((color & 0x1F) << 3));
  Background_RGB(0,0,0); 
  BTE_Operation_Code(0xF);    //BTE Operation: Memory write with Color Expansion and chroma keying (w/o ROP)  
};
//---------- setTextColor -------------------------------------------------------
void RA8876::setTextColor(uint16_t color, uint16_t bg){   // BackGround on Color
  m_textColor = color;
  m_BGtextColor = bg;
  ClearBG = 0;
  Check_BTE_Busy();
  Foreground_RGB((unsigned char)(color >> 11 << 3), (unsigned char)(((color >> 5) & 0x3F) << 2),(unsigned char)((color & 0x1F) << 3));
  Background_RGB((unsigned char)(bg >> 11 << 3), (unsigned char)(((bg >> 5) & 0x3F) << 2), (unsigned char)((bg & 0x1F) << 3));
  BTE_Operation_Code(0xE);    //BTE Operation: Memory write with Color Expansion and chroma keying (w/o ROP)  
};
//---------- SetFont -------------------------------------------------------
void RA8876::setFont(const FONT_INFO *FontTo) {
  m_fontSource = RA8876_FONT_SOURCE_EXT_GEVA;
  FontNow = FontTo;
  BTE_S0_Memory_Start_Address(OffsetBuff + FontNow->FlashStart);
#ifdef VERTICAL
  int16_t i = (FontNow->CharacterHeight+7)/8;
  int16_t m = i % 4;
  if( m ) m = 4 - m;
  BTE_S0_Image_Width(i + m );
#else
  BTE_S0_Image_Width(FontNow->BlockWidth);
#endif	
		  
};
//---------- clearScreen -------------------------------------------------------
void RA8876::clearScreen(uint16_t color){
  setCursor(0, 0);
  fillRect(0, 0, m_width, m_height, color);
  if( ClearBG ) setTextColor(m_textColor);
  else setTextColor(m_textColor, m_BGtextColor);
};
//----------------- From API  -------------------------------
void RA8876::pic_b   // Show picture from Flash
(
  const INFO* myInfo   // From enumerator generated 
  , uint32_t X1 //X of DMA Coordinate
  , uint32_t Y1 //Y of DMA Coordinate
){
  DMA_24bit(
#ifdef WriteC_0
	0 //SCS : 0 = Use SCS0, 1 = Use SCS1
#else
	1
#endif
	, 0 //Clk : SPI Clock = System Clock /{(Clk+1)*2}
	, X1 //X of DMA Coordinate
	, Y1 //Y of DMA Coordinate
	, myInfo->img_width //DMA Block width
	, myInfo->img_height //DMA Block height
	, myInfo->img_width //DMA Picture width
	, myInfo->start_addr + Pic_Offset //DMA Source Start address
  );
}
void RA8876::pic_c   // Show picture from Flash
(
  const INFO* myInfo   // From enumerator generated 
  , uint32_t X1 //X of DMA Coordinate
  , uint32_t Y1 //Y of DMA Coordinate
  , uint32_t X2 //X part image
  , uint32_t W2 //W part image
  , uint32_t H2 //H part image
){
  DMA_24bit(
#ifdef WriteC_0
	0 //SCS : 0 = Use SCS0, 1 = Use SCS1
#else
	1
#endif
	, 0 //Clk : SPI Clock = System Clock /{(Clk+1)*2}
	, X1 //X of DMA Coordinate
	, Y1 //Y of DMA Coordinate
	, W2 //DMA Block width
	, H2 //DMA Block height
	, myInfo->img_width //DMA Picture width
	, myInfo->start_addr + Pic_Offset + X2//DMA Source Start address
  );
}
void RA8876::DMA_24bit
(
  unsigned char SCS //SCS : 0 = Use SCS0, 1 = Use SCS1
  , unsigned char Clk //Clk : SPI Clock = System Clock /{(Clk+1)*2}
  , uint32_t X1 //X of DMA Coordinate
  , uint32_t Y1 //Y of DMA Coordinate
  , uint32_t X_W //DMA Block width
  , uint32_t Y_H //DMA Block height
  , uint32_t P_W //DMA Picture width
  , unsigned long Addr //DMA Source Start address
){
  //  Enable_SFlash_SPI();                     //Enable Serial Flash
  if (SCS == 0)  Select_SFI_0();                       //Select Serial Flash 0
  if (SCS == 1)  Select_SFI_1();                       //Select Serial Flash 1
  Select_SFI_DMA_Mode();                   //Set Serial Flash DMA Mode
  SPI_Clock_Period(Clk);
  /////////////////////////////////////////****************DMA
  Goto_Pixel_XY(X1, Y1);                  //set Memory coordinate in Graphic Mode
  SFI_DMA_Destination_Upper_Left_Corner(X1, Y1);      //DMA Destination position(x,y)
  SFI_DMA_Transfer_Width_Height(X_W, Y_H);        //Set DMA Block (Height , Width)
  SFI_DMA_Source_Width(P_W);                //Set DMA Source Picture Width
  SFI_DMA_Source_Start_Address(Addr);               //Set Serial Flash DMA Source Starting Address
  Start_SFI_DMA();                    //Start DMA
  Check_Busy_SFI_DMA();              //DMA Busy Check
}
void RA8876::PWM0
(
  unsigned char on_off //on_off = 1 ,enable PWM, on_off = 0 , disable PWM.
  , unsigned char Clock_Divided // divided PWM clock, only 0~3(1,1/2,1/4,1/8)
  , unsigned char Prescalar //Prescaler : only 1~256
  , unsigned short Count_Buffer //Count_Buffer : set PWM output period time
  , unsigned short Compare_Buffer //Compare_Buffer : set PWM output high level time(Duty cycle)
  /*Such as the following formula :
    PWM CLK = (Core CLK / Prescalar ) /2^ divided clock
    PWM output period = (Count Buffer + 1) x PWM CLK time
    PWM output high level time = (Compare Buffer + 1) x PWM CLK time */
){
  Select_PWM0();
  Set_PWM_Prescaler_1_to_256(Prescalar);
  if (Clock_Divided == 0)  Select_PWM0_Clock_Divided_By_1();
  if (Clock_Divided == 1)  Select_PWM0_Clock_Divided_By_2();
  if (Clock_Divided == 2)  Select_PWM0_Clock_Divided_By_4();
  if (Clock_Divided == 3)  Select_PWM0_Clock_Divided_By_8();
  Set_Timer0_Count_Buffer(Count_Buffer);
  Set_Timer0_Compare_Buffer(Compare_Buffer);
  if (on_off == 1) Start_PWM0();
  if (on_off == 0) Stop_PWM0();
}
