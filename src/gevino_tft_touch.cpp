/*
  GEVINO TFT - capacitive touch support (7" FT5206 / 10" GT9271)
  GEVA Elettronica - https://www.gevaelettronica.it

  See gevino_tft_touch.h for the public API. Both controllers are driven
  directly over Wire, so no external touch library is required.
*/

#include "gevino_tft_touch.h"

// Panel detected by gevino_touch_setup()
static uint8_t s_nPanel = GEVINO_TOUCH_NONE;

// -----------------------------------------------------------------------------
// Small I2C helpers
// -----------------------------------------------------------------------------

// Does anything answer at this address? Written the same way for both
// controllers: address the first register, then ask for one byte back.
static bool gevino_touch_probe(uint8_t nAddr)
{
  Wire.beginTransmission(nAddr);
  Wire.write((uint8_t)0);
  Wire.write((uint8_t)0);
  Wire.endTransmission();

  Wire.requestFrom(nAddr, (uint8_t)1);
  bool bFound = (Wire.available() > 0);
  while (Wire.available()) { Wire.read(); }   // leave the buffer clean
  return bFound;
}

// --- FT5206 (7"): 8-bit register addresses ---
static uint8_t ft5206_read(uint8_t nReg, uint8_t* pBuf, uint8_t nLen)
{
  Wire.beginTransmission((uint8_t)GEVINO_TOUCH_FT5206_ADDR);
  Wire.write(nReg);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)GEVINO_TOUCH_FT5206_ADDR, nLen);
  uint8_t i;
  for (i = 0; (i < nLen) && Wire.available(); i++) { pBuf[i] = Wire.read(); }
  return i;
}

// --- GT9271 (10"): 16-bit register addresses ---
static uint8_t gt9271_read(uint16_t nReg, uint8_t* pBuf, uint8_t nLen)
{
  Wire.beginTransmission((uint8_t)GEVINO_TOUCH_GT9271_ADDR);
  Wire.write((uint8_t)(nReg >> 8));
  Wire.write((uint8_t)(nReg & 0xFF));
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)GEVINO_TOUCH_GT9271_ADDR, nLen);
  uint8_t i;
  for (i = 0; (i < nLen) && Wire.available(); i++) { pBuf[i] = Wire.read(); }
  return i;
}

static void gt9271_write(uint16_t nReg, const uint8_t* pVal, uint16_t nCnt)
{
  Wire.beginTransmission((uint8_t)GEVINO_TOUCH_GT9271_ADDR);
  Wire.write((uint8_t)(nReg >> 8));
  Wire.write((uint8_t)(nReg & 0xFF));
  for (uint16_t i = 0; i < nCnt; i++, pVal++) { Wire.write(*pVal); }
  Wire.endTransmission();
}

// Both panels are mounted rotated 180 deg with respect to the display memory,
// so both axes are flipped; the result is clamped to the visible area.
static void gevino_touch_map(uint16_t nRawX, uint16_t nRawY, GevinoTouchPoint* pLoc)
{
  int32_t x = (int32_t)GEVINO_TOUCH_WIDTH  - (int32_t)nRawX;
  int32_t y = (int32_t)GEVINO_TOUCH_HEIGHT - (int32_t)nRawY;

  if (x < 0) { x = 0; }
  if (x > GEVINO_TOUCH_WIDTH  - 1) { x = GEVINO_TOUCH_WIDTH  - 1; }
  if (y < 0) { y = 0; }
  if (y > GEVINO_TOUCH_HEIGHT - 1) { y = GEVINO_TOUCH_HEIGHT - 1; }

  pLoc->x = (int16_t)x;
  pLoc->y = (int16_t)y;
}

// -----------------------------------------------------------------------------
// Detection / setup
// -----------------------------------------------------------------------------

uint8_t gevino_touch_setup(void)
{
  Wire.begin();

  s_nPanel = GEVINO_TOUCH_NONE;
  if      (gevino_touch_probe(GEVINO_TOUCH_FT5206_ADDR)) { s_nPanel = GEVINO_TOUCH_7;  }
  else if (gevino_touch_probe(GEVINO_TOUCH_GT9271_ADDR)) { s_nPanel = GEVINO_TOUCH_10; }

  gevino_touch_pin_setup();
  return s_nPanel;
}

void gevino_touch_pin_setup(void)
{
  if (s_nPanel == GEVINO_TOUCH_NONE) { return; }

  // Drive the shared INT line low before releasing it: the controller then
  // takes it over and pulls it low on every touch frame.
  digitalWrite(TouchInt, LOW);
  pinMode(TouchInt, (s_nPanel == GEVINO_TOUCH_10) ? INPUT : INPUT_PULLUP);
}

uint8_t     gevino_touch_panel(void) { return s_nPanel; }
bool        gevino_touch_found(void) { return s_nPanel != GEVINO_TOUCH_NONE; }
bool        gevino_touch_is7  (void) { return s_nPanel == GEVINO_TOUCH_7;  }
bool        gevino_touch_is10 (void) { return s_nPanel == GEVINO_TOUCH_10; }

const char* gevino_touch_name (void)
{
  if (s_nPanel == GEVINO_TOUCH_7)  { return "7\"";  }
  if (s_nPanel == GEVINO_TOUCH_10) { return "10\""; }
  return "none";
}

// -----------------------------------------------------------------------------
// Reading
// -----------------------------------------------------------------------------

bool gevino_touch_down(void)
{
  if (s_nPanel == GEVINO_TOUCH_NONE) { return false; }
  return (digitalRead(TouchInt) == LOW);
}

uint8_t gevino_touch_points(GevinoTouchPoint* pLoc, uint8_t nMax)
{
  if (!pLoc || !nMax) { return 0; }
  if (s_nPanel == GEVINO_TOUCH_NONE) { return 0; }

  // Both controllers hold the line low only while a frame is pending: skipping
  // the I2C read when it is high keeps the bus (and loop()) free.
  if (digitalRead(TouchInt) != LOW) { return 0; }

  if (s_nPanel == GEVINO_TOUCH_7) {
    // ---- FT5206: point k is 6 bytes at 0x03 + k*6 ----
    uint8_t nStatus = 0;
    if (ft5206_read(0x02, &nStatus, 1) != 1) { return 0; }

    uint8_t nHits = nStatus & 0x0F;
    if ((nHits == 0) || (nHits > 5)) { return 0; }
    if (nHits > nMax) { nHits = nMax; }

    uint8_t aBuf[5 * 6];
    if (ft5206_read(0x03, aBuf, (uint8_t)(nHits * 6)) < (uint8_t)(nHits * 6)) { return 0; }

    for (uint8_t k = 0; k < nHits; k++) {
      const uint8_t* p = &aBuf[k * 6];
      // XH/YH carry the event flag and the finger id in their top nibble
      gevino_touch_map((uint16_t)(((p[0] & 0x0F) << 8) | p[1]),
                       (uint16_t)(((p[2] & 0x0F) << 8) | p[3]), &pLoc[k]);
      pLoc[k].id = (uint8_t)(p[2] >> 4);
    }
    return nHits;
  }

  // ---- GT9271: status at 0x814E, point k is 8 bytes with x at 0x8150 + k*8 ----
  uint8_t nStatus = 0;
  gt9271_read(0x814E, &nStatus, 1);

  uint8_t nHits = nStatus & 0x0F;
  if (nHits > GEVINO_TOUCH_MAX_POINTS) { nHits = GEVINO_TOUCH_MAX_POINTS; }
  if (nHits > nMax) { nHits = nMax; }

  for (uint8_t k = 0; k < nHits; k++) {
    uint8_t aBuf[5];
    // 0x814F + k*8 is the finger id, so read from there and take x/y after it
    if (gt9271_read((uint16_t)(0x814F + k * 8), aBuf, 5) < 5) { nHits = k; break; }
    gevino_touch_map((uint16_t)((aBuf[2] << 8) | aBuf[1]),
                     (uint16_t)((aBuf[4] << 8) | aBuf[3]), &pLoc[k]);
    pLoc[k].id = aBuf[0];
  }

  // Clearing the status register is what lets the controller pull INT low
  // again on the next frame - do it even when there were no points.
  uint8_t nZero = 0;
  gt9271_write(0x814E, &nZero, 1);

  return nHits;
}

bool gevino_touch_read(int16_t* pX, int16_t* pY)
{
  GevinoTouchPoint sLoc;
  if (gevino_touch_points(&sLoc, 1) < 1) { return false; }
  if (pX) { *pX = sLoc.x; }
  if (pY) { *pY = sLoc.y; }
  return true;
}
