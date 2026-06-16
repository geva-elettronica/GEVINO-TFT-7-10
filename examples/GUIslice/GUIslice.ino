//
// GUIslice Library Example for the GEVINO TFT (7" / 10")
// - GEVA Elettronica - https://www.gevaelettronica.it
//
//   First, simple step of the GUIslice port to the GEVINO TFT HMI PLC:
//   display driver + basic GUIslice commands (box, text, buttons, touch),
//   wired together with the GEVINO board I/O (front LED, opto Out0, opto In0).
//
//   Hardware:
//     - GEVINO TFT, Arduino Zero compatible (SAMD21)
//     - 1024x600 TFT, Raio RA8876 controller (SPI, TFT_CS = 42)
//     - 7" capacitive touch: FT5206 (I2C 0x38, INT on D38)
//
//   Expected behaviour:
//     - The screen shows a title, a live counter, the In0 input state and
//       three buttons.
//     - "LED ON" / "LED OFF" toggle the front yellow LED.
//     - "OUT0" toggles the opto-isolated output Out0 and updates its label.
//
// =============================================================================
// PREREQUISITES (Arduino IDE / arduino-cli):
//   1. Board:    "Arduino Zero (Native USB Port)" (Arduino SAMD core).
//   2. Libraries installed alongside GUIslice:
//        - GEVINO TFT library  (provides gevino_tft_io.h + gevinoTFT_RA8876)
//        - FT5206              (sumotoy/FT5206 capacitive touch)
//   3. Config:   in GUIslice/src/GUIslice_config.h enable the line
//        #include "../configs/ard-shld-gevino_tft.h"
//      (this selects DRV_DISP_ADAGFX_RA8876_GV + DRV_TOUCH_ADA_FT5206).
//
// NOTE: this example uses the GEVA external fonts that live in the display
//       serial flash (notoSans_32pt). The glyphs must already be programmed
//       into the flash (font.bin); RA8876::init() copies them into the
//       controller canvas RAM at startup. Images are still the pending step.
// =============================================================================

// GEVINO TFT board I/O (SPI, chip-selects, FT5206 reset, In/Out macros,
// updateIO(), the Timer class). Include it in exactly ONE translation unit
// (this .ino): it instantiates Serial2/Serial3 and the global I/O registers.
#include "gevino_tft_io.h"

// GEVA flash images — the "glass" theme. Provides the BINARY_INFO table and the
// PICTURE_NAME enum (glass_rect_*, glass_round_* in pressed/released states).
// Included in the canonical order (io -> images -> display): gevino_tft_io.h has
// defined `pic_def`, so this header supplies the INFO typedef and
// gevinoTFT_RA8876.h skips its own (avoids the duplicate-typedef vs. undef dance).
#include "glass/All_76_Pic_65K.h"

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Extended elements
#include "elem/XSlider.h"

// GEVA external fonts stored in the display serial flash (descriptors only; the
// glyphs are copied into canvas RAM by RA8876::init()). fontExt.h declares them,
// font.h (orientation-specific) defines them. Horizontal 1024x600 set here.
#include "fontExt.h"
#include "Horizzontal/font.h"

// Accessor exported by the GUIslice RA8876_GV driver: the fully-initialized
// display instance, used here for native flash-image DMA blits via pic().
extern "C" RA8876* gslc_GevinoDisp(void);

// Theme: background 0x2390ec, white text
#define GEVA_BLUE_565   RGB565(0x23, 0x90, 0xEC)
#define GEVA_BLUE_COL   ((gslc_tsColor){ 0x23, 0x90, 0xEC })

// Darker shade used to dim the welcome splash background
#define GEVA_DARK_COL   ((gslc_tsColor){ 0x0A, 0x1C, 0x2E })

// -----------------------------------------------------------------------------
// Enumerations for pages, elements and fonts
// -----------------------------------------------------------------------------
// Pages
enum { E_PG_WELCOME, E_PG_MAIN, E_PG_FONTS };

// Welcome (splash) elements. E_W_BG is a full-screen dark box drawn first so it
// darkens the blue background; the texts are drawn on top of it.
enum { E_W_BG, E_W_TITLE, E_W_SUB, E_W_URL, E_W_THEME };

// Main-page elements. The 8 output-LED IDs are contiguous so (nId - E_LED0)
// gives the output index 0..7; likewise the 8 input-LED IDs (nId - E_LEDI0).
enum { E_BOX, E_TITLE, E_TXT_COUNT, E_TXT_IN0,
       E_BTN_LED_ON, E_BTN_LED_OFF, E_BTN_OUT0,
       E_LBL_DAC, E_SLIDER_DAC, E_TXT_DAC, E_BTN_FONTS, E_BTN_MODE,
       E_LBL_OUT, E_LED0, E_LED1, E_LED2, E_LED3, E_LED4, E_LED5, E_LED6, E_LED7,
       E_LBL_IN, E_LEDI0, E_LEDI1, E_LEDI2, E_LEDI3, E_LEDI4, E_LEDI5, E_LEDI6, E_LEDI7 };

// Font-gallery page elements. The 7 family tab IDs are contiguous so that
// (nId - E_TAB_SANS) gives the family index 0..6.
enum { E_FS_NAME,
       E_TAB_SANS, E_TAB_BOLD, E_TAB_ITAL, E_TAB_MONO, E_TAB_MONOB, E_TAB_SERIF, E_TAB_SERIFB,
       E_BTN_BACK,
       E_FS_S0, E_FS_S1, E_FS_S2, E_FS_S3, E_FS_S4, E_FS_S5, E_FS_S6, E_FS_S7, E_FS_S8 };

// Font slots. E_FONT_FS0..FS8 are 9 reusable slots, re-pointed at run time to
// the selected family's sizes (see ShowFamily). Must be contiguous.
enum { E_FONT_TITLE, E_FONT_TXT, E_FONT_TAB, E_FONT_WBIG, E_FONT_ITAL,
       E_FONT_FS0, E_FONT_FS1, E_FONT_FS2, E_FONT_FS3, E_FONT_FS4,
       E_FONT_FS5, E_FONT_FS6, E_FONT_FS7, E_FONT_FS8, MAX_FONT };  // MAX_FONT last

// -----------------------------------------------------------------------------
// GUIslice instances
// -----------------------------------------------------------------------------
#define MAX_PAGE             3
#define MAX_ELEM_PG_WELCOME  5
#define MAX_ELEM_PG_MAIN     30
#define MAX_ELEM_PG_FONTS    18

gslc_tsGui      m_gui;
gslc_tsDriver   m_drv;
gslc_tsFont     m_asFont[MAX_FONT];
gslc_tsPage     m_asPage[MAX_PAGE];
gslc_tsElem     m_asElemWelcome[MAX_ELEM_PG_WELCOME];
gslc_tsElemRef  m_asElemRefWelcome[MAX_ELEM_PG_WELCOME];
gslc_tsElem     m_asElemMain[MAX_ELEM_PG_MAIN];
gslc_tsElemRef  m_asElemRefMain[MAX_ELEM_PG_MAIN];
gslc_tsElem     m_asElemFonts[MAX_ELEM_PG_FONTS];
gslc_tsElemRef  m_asElemRefFonts[MAX_ELEM_PG_FONTS];

gslc_tsXSlider  m_sSliderDac;   // storage for the slider extended element

// References to the elements we update at run time
gslc_tsElemRef* m_pTxtCount   = NULL;
gslc_tsElemRef* m_pTxtIn0     = NULL;
gslc_tsElemRef* m_pBtnOut0    = NULL;
gslc_tsElemRef* m_pTxtDac     = NULL;
gslc_tsElemRef* m_pSlider     = NULL;
gslc_tsElemRef* m_pBtnMode    = NULL;
gslc_tsElemRef* m_pFamName    = NULL;
gslc_tsElemRef* m_apSample[9] = { NULL };
gslc_tsElemRef* m_apLed[8]    = { NULL };   // 8 on-screen output LEDs (Out0..Out7)
gslc_tsElemRef* m_apLedIn[8]  = { NULL };   // 8 on-screen input LEDs  (In0..In7)

// External text buffers (config uses GSLC_LOCAL_STR = 0, so any text we want
// to change at run time must point to a writable buffer with bufMax > 0)
char m_acTxtCount[20] = "";
char m_acTxtIn0[24]   = "";
char m_acBtnOut0[12]  = "OUT0: OFF";
char m_acTxtDac[16]   = "DAC: 0%";
char m_acBtnMode[10]  = "ANALOG";
char m_acFamName[24]  = "Noto Sans";

int16_t m_nDacPos = 0;          // slider position 0..100

uint32_t m_nCount = 0;
Timer    m_tSec(1000);       // GEVINO non-blocking 1-second tick

bool     m_bWelcome = true;  // showing the splash?
Timer    m_tWelcome(5000);   // 5-second welcome screen

// --- ANALOG / DIGITAL mode and the 8-light show on Out0..Out7 ---
enum  { MODE_ANALOG, MODE_DIGITAL };
uint8_t m_eMode = MODE_ANALOG;

uint8_t m_nLightSel  = 0;        // selected light 0..7 (from slider position)
bool    m_bHeldPrev  = false;    // was the slider held last loop?
Timer   m_tStill(500);           // slider held still this long -> start the chase
Timer   m_tChase(70);            // chase step while held still
uint8_t m_nChaseIdx  = 0;
bool    m_bSettle    = false;    // running the release oscillation?
uint8_t m_nSettleStep = 0;
Timer   m_tSettle(150);          // oscillation step on release
const int8_t SETTLE_OFFS[5] = { -1, -2, +1, +2, 0 };   // -1,-2 then +1,+2 then back

// Digital-light helpers (8 lights on Out0..Out7; Out0 is direct GPIO, Out1..7 shift register)
static uint8_t posToIdx(int16_t pos) { int16_t i = (int16_t)pos * 8 / 100; return (uint8_t)(i < 0 ? 0 : i > 7 ? 7 : i); }
static uint8_t clampIdx(int16_t i)   { return (uint8_t)(i < 0 ? 0 : i > 7 ? 7 : i); }

static bool outState(uint8_t i) {            // current state of output Out0..Out7
  switch (i) {
    case 0: return Out0;  case 1: return Out1;  case 2: return Out2;  case 3: return Out3;
    case 4: return Out4;  case 5: return Out5;  case 6: return Out6;  case 7: return Out7;
  }
  return false;
}
static void refreshLeds(void) {              // queue the 8 on-screen LEDs for redraw
  for (uint8_t i = 0; i < 8; i++) {
    if (m_apLed[i]) gslc_ElemSetRedraw(&m_gui, m_apLed[i], GSLC_REDRAW_INC);
  }
}
static bool inState(uint8_t i) {             // current state of input In0..In7
  switch (i) {
    case 0: return In0;  case 1: return In1;  case 2: return In2;  case 3: return In3;
    case 4: return In4;  case 5: return In5;  case 6: return In6;  case 7: return In7;
  }
  return false;
}
static void refreshLedsIn(void) {            // queue the 8 on-screen input LEDs for redraw
  for (uint8_t i = 0; i < 8; i++) {
    if (m_apLedIn[i]) gslc_ElemSetRedraw(&m_gui, m_apLedIn[i], GSLC_REDRAW_INC);
  }
}
static void allLightsOff(void) {
  resOut0; resOut1; resOut2; resOut3; resOut4; resOut5; resOut6; resOut7;
  updateIO();
  refreshLeds();
}
static void setLight(uint8_t idx) {          // light exactly one of Out0..Out7
  resOut0; resOut1; resOut2; resOut3; resOut4; resOut5; resOut6; resOut7;
  switch (idx) {
    case 0: setOut0; break;  case 1: setOut1; break;
    case 2: setOut2; break;  case 3: setOut3; break;
    case 4: setOut4; break;  case 5: setOut5; break;
    case 6: setOut6; break;  case 7: setOut7; break;
  }
  updateIO();
  refreshLeds();
}
static void showValueText(void) {            // update the value label for the current mode
  char acTxt[16];
  if (m_eMode == MODE_ANALOG) snprintf(acTxt, sizeof(acTxt), "DAC: %d%%", m_nDacPos);
  else                        snprintf(acTxt, sizeof(acTxt), "Light: %d", m_nLightSel + 1);
  gslc_ElemSetTxtStr(&m_gui, m_pTxtDac, acTxt);
}

// -----------------------------------------------------------------------------
// Font-gallery data (all in flash). 7 families x 9 sizes, defined in font.h.
// -----------------------------------------------------------------------------
const FONT_INFO* const FAM_SANS[9]    = { &notoSans_8pt, &notoSans_12pt, &notoSans_20pt, &notoSans_32pt, &notoSans_48pt, &notoSans_64pt, &notoSans_80pt, &notoSans_96pt, &notoSans_112pt };
const FONT_INFO* const FAM_SANS_B[9]  = { &notoSansBold_8pt, &notoSansBold_12pt, &notoSansBold_20pt, &notoSansBold_32pt, &notoSansBold_48pt, &notoSansBold_64pt, &notoSansBold_80pt, &notoSansBold_96pt, &notoSansBold_112pt };
const FONT_INFO* const FAM_SANS_I[9]  = { &notoSansItal_8pt, &notoSansItal_12pt, &notoSansItal_20pt, &notoSansItal_32pt, &notoSansItal_48pt, &notoSansItal_64pt, &notoSansItal_80pt, &notoSansItal_96pt, &notoSansItal_112pt };
const FONT_INFO* const FAM_MONO[9]    = { &notoSansMono_8pt, &notoSansMono_12pt, &notoSansMono_20pt, &notoSansMono_32pt, &notoSansMono_48pt, &notoSansMono_64pt, &notoSansMono_80pt, &notoSansMono_96pt, &notoSansMono_112pt };
const FONT_INFO* const FAM_MONO_B[9]  = { &notoSansMonoBold_8pt, &notoSansMonoBold_12pt, &notoSansMonoBold_20pt, &notoSansMonoBold_32pt, &notoSansMonoBold_48pt, &notoSansMonoBold_64pt, &notoSansMonoBold_80pt, &notoSansMonoBold_96pt, &notoSansMonoBold_112pt };
const FONT_INFO* const FAM_SERIF[9]   = { &notoSerif_8pt, &notoSerif_12pt, &notoSerif_20pt, &notoSerif_32pt, &notoSerif_48pt, &notoSerif_64pt, &notoSerif_80pt, &notoSerif_96pt, &notoSerif_112pt };
const FONT_INFO* const FAM_SERIF_B[9] = { &notoSerifBold_8pt, &notoSerifBold_12pt, &notoSerifBold_20pt, &notoSerifBold_32pt, &notoSerifBold_48pt, &notoSerifBold_64pt, &notoSerifBold_80pt, &notoSerifBold_96pt, &notoSerifBold_112pt };

const FONT_INFO* const* const FAMILIES[7] = { FAM_SANS, FAM_SANS_B, FAM_SANS_I, FAM_MONO, FAM_MONO_B, FAM_SERIF, FAM_SERIF_B };
const char* const FAM_NAMES[7]  = { "Noto Sans", "Noto Sans Bold", "Noto Sans Italic", "Noto Sans Mono", "Noto Sans Mono Bold", "Noto Serif", "Noto Serif Bold" };
const char* const TAB_LABELS[7] = { "Sans", "Bold", "Ital", "Mono", "MonoB", "Serif", "SerifB" };

// One sample string per size slot (short for the big sizes so they stay on screen)
const char* const FS_SAMPLE[9] = { "8 Ag Bb", "12 Ag Bb", "20 Ag Bb", "32 Ag Bb", "48 Ag", "64 Ag", "80 Ag", "96 Ag", "112" };

// Layout of the 9 sample lines: small sizes stacked in the left column,
// the three biggest in the right column (1024x600).
const int16_t FS_X[9] = {  20,  20,  20,  20,  20,  20, 540, 540, 540 };
const int16_t FS_Y[9] = { 110, 140, 176, 226, 298, 400, 110, 268, 450 };
const int16_t FS_W[9] = { 500, 500, 500, 500, 500, 500, 470, 470, 470 };
const int16_t FS_H[9] = {  24,  30,  44,  66,  96, 128, 150, 176, 145 };

// Forward declarations (referenced before their definition)
void ShowFamily(uint8_t nFam);
bool CbTab(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY);
bool CbDrawBtnBig(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);
bool CbDrawBtnTab(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);
bool CbDrawSlider(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);
bool CbDrawLed(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);
bool CbDrawLedIn(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw);
void InitPageWelcome(void);
void InitPageFonts(void);

// -----------------------------------------------------------------------------
// Route GUIslice debug output to the rear USB-C console (SerialUSB)
// -----------------------------------------------------------------------------
static int16_t DebugOut(char ch) { SerialUSB.write(ch); return 0; }

// -----------------------------------------------------------------------------
// Common button callback
// -----------------------------------------------------------------------------
bool CbBtn(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  (void)nX; (void)nY;
  gslc_tsGui*     pGui     = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui, pElemRef);

  if (eTouch == GSLC_TOUCH_UP_IN) {
    switch (pElem->nId) {
      case E_BTN_LED_ON:
        setLed;                                   // front yellow LED ON
        break;
      case E_BTN_LED_OFF:
        resLed;                                   // front yellow LED OFF
        break;
      case E_BTN_OUT0:
        if (Out0) {                               // opto output Out0 toggle
          resOut0;
          gslc_ElemSetTxtStr(pGui, m_pBtnOut0, "OUT0: OFF");
        } else {
          setOut0;
          gslc_ElemSetTxtStr(pGui, m_pBtnOut0, "OUT0: ON");
        }
        refreshLeds();                            // mirror Out0 on the LED row
        break;
      case E_BTN_FONTS:
        gslc_SetPageCur(pGui, E_PG_FONTS);        // open the font gallery
        ShowFamily(0);                            // default to Noto Sans
        break;
      case E_BTN_MODE:
        if (m_eMode == MODE_ANALOG) {
          m_eMode = MODE_DIGITAL;
          gslc_ElemSetTxtStr(pGui, m_pBtnMode, "DIGITAL");
          setLight(m_nLightSel);                          // show the selected light
          m_tStill.reset();
          m_bHeldPrev = false;
        } else {
          m_eMode = MODE_ANALOG;
          gslc_ElemSetTxtStr(pGui, m_pBtnMode, "ANALOG");
          allLightsOff();
          analogWrite(DAC0, (m_nDacPos * 1023) / 100);    // restore the analog output
        }
        m_bSettle = false;
        showValueText();
        break;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
// Point the 9 reusable sample font slots at the chosen family's sizes and
// repaint the gallery page (re-pointing a font slot at run time is cheap:
// gslc_FontSet just rewrites the slot, gslc_DrvFontAdd returns the pointer).
// -----------------------------------------------------------------------------
void ShowFamily(uint8_t nFam)
{
  if (nFam >= 7) { return; }
  const FONT_INFO* const* fam = FAMILIES[nFam];
  for (uint8_t i = 0; i < 9; i++) {
    gslc_FontSet(&m_gui, E_FONT_FS0 + i, GSLC_FONTREF_PTR, fam[i], 1);
  }
  gslc_ElemSetTxtStr(&m_gui, m_pFamName, FAM_NAMES[nFam]);
  // Fonts changed, so force a full repaint: clears the previous (often larger)
  // glyphs and re-measures every sample with its new font.
  // - gslc_PageRedrawSet() alone only flags the redraw; the actual repaint is
  //   clipped to the invalidation region (which would only cover the family
  //   name we just changed). Invalidate the WHOLE screen so the background is
  //   repainted everywhere and the previous samples are erased.
  gslc_InvalidateRgnScreen(&m_gui);
  gslc_PageRedrawSet(&m_gui, true);
}

// -----------------------------------------------------------------------------
// Font-gallery tab / back callback
// -----------------------------------------------------------------------------
bool CbTab(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  (void)nX; (void)nY;
  gslc_tsGui*  pGui  = (gslc_tsGui*)pvGui;
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, (gslc_tsElemRef*)pvElemRef);

  if (eTouch == GSLC_TOUCH_UP_IN) {
    int16_t nId = pElem->nId;
    if (nId == E_BTN_BACK) {
      gslc_SetPageCur(pGui, E_PG_MAIN);
    } else if ((nId >= E_TAB_SANS) && (nId <= E_TAB_SERIFB)) {
      ShowFamily((uint8_t)(nId - E_TAB_SANS));
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
// Custom button drawing: blit a glass image (pressed/released) from the display
// flash and render the (white) label centred on top. The button rect MUST match
// the image size, since the DMA blit is unscaled.
// -----------------------------------------------------------------------------
static void DrawGlassBtn(void* pvGui, void* pvElemRef,
                         uint16_t nImgRel, uint16_t nImgPrs,
                         const FONT_INFO* pLabelFont, int16_t nW, int16_t nH)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui, pElemRef);
  RA8876*         tft      = gslc_GevinoDisp();

  int16_t x = pElem->rElem.x;
  int16_t y = pElem->rElem.y;
  bool    bGlow = gslc_ElemGetGlow(pGui, pElemRef);

  // Glass background (opaque DMA blit, fills the whole button)
  tft->pic_b(&BINARY_INFO[bGlow ? nImgPrs : nImgRel], x, y);

  // Centred white label (transparent/chroma-keyed background)
  const char* str = pElem->pStrBuf;
  if (str && str[0]) {
    int16_t  bx, by;
    uint16_t bw, bh;
    tft->setFont(pLabelFont);
    tft->setTextColor(WHITE);
    tft->getTextBounds(str, 0, 0, &bx, &by, &bw, &bh);
    tft->setCursor(x + (nW - (int16_t)bw) / 2, y + (nH - (int16_t)bh) / 2);
    tft->print(str);
  }

  // A custom draw callback is responsible for clearing the redraw flag
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
}

bool CbDrawBtnBig(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  (void)eRedraw;
  DrawGlassBtn(pvGui, pvElemRef, glass_rect_251_70_released, glass_rect_251_70_pressed,
               &notoSans_20pt, 251, 70);
  return true;
}

bool CbDrawBtnTab(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  (void)eRedraw;
  DrawGlassBtn(pvGui, pvElemRef, glass_rect_105_55_released, glass_rect_105_55_pressed,
               &notoSans_12pt, 105, 55);
  return true;
}

// -----------------------------------------------------------------------------
// Custom slider drawing: blue background + thin track + the smallest glass round
// image (31x31) as the thumb. The XSlider still handles the touch/drag and the
// position callback; we only override the drawing.
// -----------------------------------------------------------------------------
bool CbDrawSlider(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  (void)eRedraw;
  gslc_tsGui*     pGui     = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui, pElemRef);
  RA8876*         tft      = gslc_GevinoDisp();

  const int16_t kThumb = 31;   // glass_round_31 size
  int16_t x = pElem->rElem.x;
  int16_t y = pElem->rElem.y;
  int16_t w = pElem->rElem.w;
  int16_t h = pElem->rElem.h;
  int16_t pos = gslc_ElemXSliderGetPos(pGui, pElemRef);   // 0..100

  // Background (erase previous thumb)
  tft->fillRect(x, y, x + w - 1, y + h - 1, GEVA_BLUE_565);

  // Track
  int16_t ty = y + h / 2;
  tft->fillRect(x + kThumb / 2, ty - 2, x + w - 1 - kThumb / 2, ty + 2, WHITE);

  // Thumb = smallest glass round, positioned by value
  int16_t travel = w - kThumb;
  int16_t tx = x + (int16_t)((int32_t)pos * travel / 100);
  bool    bGlow = gslc_ElemGetGlow(pGui, pElemRef);
  tft->pic_b(&BINARY_INFO[bGlow ? glass_round_31_pressed : glass_round_31_released],
             tx, y + (h - kThumb) / 2);

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}

// -----------------------------------------------------------------------------
// Output-LED drawing: blit the red glass LED (lit/off) for output Out0..Out7
// -----------------------------------------------------------------------------
bool CbDrawLed(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  (void)eRedraw;
  gslc_tsGui*     pGui     = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui, pElemRef);
  RA8876*         tft      = gslc_GevinoDisp();

  uint8_t idx = (uint8_t)(pElem->nId - E_LED0);   // 0..7
  tft->pic_b(&BINARY_INFO[outState(idx) ? glass_round_31_led_red : glass_round_31_led_off],
             pElem->rElem.x, pElem->rElem.y);

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}

// -----------------------------------------------------------------------------
// Input-LED drawing: blit the yellow glass LED (lit/off) for input In0..In7
// -----------------------------------------------------------------------------
bool CbDrawLedIn(void* pvGui, void* pvElemRef, gslc_teRedrawType eRedraw)
{
  (void)eRedraw;
  gslc_tsGui*     pGui     = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui, pElemRef);
  RA8876*         tft      = gslc_GevinoDisp();

  uint8_t idx = (uint8_t)(pElem->nId - E_LEDI0);   // 0..7
  tft->pic_b(&BINARY_INFO[inState(idx) ? glass_round_31_led_yellow : glass_round_31_led_off],
             pElem->rElem.x, pElem->rElem.y);

  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_NONE);
  return true;
}

// -----------------------------------------------------------------------------
// Slider position callback
// - Drives the GEVINO 10-bit analog output (DAC) and updates the value label
// -----------------------------------------------------------------------------
bool CbSlider(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui, pElemRef);

  if (pElem->nId == E_SLIDER_DAC) {
    m_nDacPos = nPos;                                  // 0..100
    if (m_eMode == MODE_ANALOG) {
      analogWrite(DAC0, (m_nDacPos * 1023) / 100);     // 10-bit DAC: 0..1023
    } else {
      m_nLightSel = posToIdx(m_nDacPos);               // moving -> the lit light follows the slider
      setLight(m_nLightSel);
      m_tStill.reset();                                // movement postpones the "held still" chase
    }
    showValueText();
  }
  return true;
}

// -----------------------------------------------------------------------------
// setup()
// -----------------------------------------------------------------------------
void setup()
{
  SerialUSB.begin(115200);    // rear USB-C debug console
  Serial2.begin(115200);      // extra SERCOM UART (instantiated by the board header)
  Serial3.begin(115200);      // extra SERCOM UART

  // GEVINO board bring-up: configures SPI + drive strength, raises all the
  // shared-bus chip-selects (SD/ETH/CH376) and resets the FT5206 touch panel.
  // Call it once, after Serial2/Serial3.begin().
  gevino_io_setup();

  gslc_InitDebug(&DebugOut);

  // Initialize GUIslice (this also runs RA8876::init() and FT5206::begin())
  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) { return; }

  // Fonts: use the GEVA external font notoSans_32pt (in display flash).
  // For external GEVA fonts the glyphs are blitted at their native size, so the
  // size/scale argument is not used (kept at 1). To enlarge a label, point it at
  // a bigger flash font instead (e.g. &notoSans_20pt / &notoSans_32pt).
  if (!gslc_FontSet(&m_gui, E_FONT_TITLE, GSLC_FONTREF_PTR, &notoSans_32pt,     1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_TXT,   GSLC_FONTREF_PTR, &notoSans_32pt,     1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_TAB,   GSLC_FONTREF_PTR, &notoSans_20pt,     1)) { return; }
  if (!gslc_FontSet(&m_gui, E_FONT_WBIG,  GSLC_FONTREF_PTR, &notoSans_48pt,     1)) { return; }  // welcome title
  if (!gslc_FontSet(&m_gui, E_FONT_ITAL,  GSLC_FONTREF_PTR, &notoSansItal_20pt, 1)) { return; }  // "glass theme"
  // 9 reusable sample slots, initialised to Noto Sans (re-pointed by ShowFamily)
  for (uint8_t i = 0; i < 9; i++) {
    if (!gslc_FontSet(&m_gui, E_FONT_FS0 + i, GSLC_FONTREF_PTR, FAM_SANS[i], 1)) { return; }
  }

  // -----------------------------------
  // Build the main page
  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asElemMain, MAX_ELEM_PG_MAIN,
               m_asElemRefMain, MAX_ELEM_PG_MAIN);

  gslc_SetBkgndColor(&m_gui, GEVA_BLUE_COL);   // 0x2390ec

  gslc_tsElemRef* pElemRef;

  // Outer frame box (transparent fill so the blue background shows through)
  pElemRef = gslc_ElemCreateBox(&m_gui, E_BOX, E_PG_MAIN, (gslc_tsRect){ 20, 20, 984, 560 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GEVA_BLUE_COL, GEVA_BLUE_COL);
  gslc_ElemSetFillEn(&m_gui, pElemRef, false);

  // Title
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_TITLE, E_PG_MAIN, (gslc_tsRect){ 60, 40, 640, 60 },
                                (char*)"GEVINO TFT + GUIslice", 0, E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GEVA_BLUE_COL, GEVA_BLUE_COL, GEVA_BLUE_COL);  // blue fill, not black

  // Live counter (updated every second)
  m_pTxtCount = gslc_ElemCreateTxt(&m_gui, E_TXT_COUNT, E_PG_MAIN, (gslc_tsRect){ 60, 160, 600, 40 },
                                   m_acTxtCount, sizeof(m_acTxtCount), E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, m_pTxtCount, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, m_pTxtCount, GEVA_BLUE_COL, GEVA_BLUE_COL, GEVA_BLUE_COL);

  // Input In0 state
  m_pTxtIn0 = gslc_ElemCreateTxt(&m_gui, E_TXT_IN0, E_PG_MAIN, (gslc_tsRect){ 60, 220, 600, 40 },
                                 m_acTxtIn0, sizeof(m_acTxtIn0), E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, m_pTxtIn0, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, m_pTxtIn0, GEVA_BLUE_COL, GEVA_BLUE_COL, GEVA_BLUE_COL);

  // Button to open the font gallery (glass image button) - top right
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_FONTS, E_PG_MAIN, (gslc_tsRect){ 730, 30, 251, 70 },
                                   (char*)"FONTS", 0, E_FONT_TXT, &CbBtn);
  gslc_ElemSetDrawFunc(&m_gui, pElemRef, &CbDrawBtnBig);

  // 8 output LEDs (Out0..Out7), red glass symbols, in the space freed by FONTS
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_LBL_OUT, E_PG_MAIN, (gslc_tsRect){ 700, 135, 300, 36 },
                                (char*)"Outputs 1-8", 0, E_FONT_TAB);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GEVA_BLUE_COL, GEVA_BLUE_COL, GEVA_BLUE_COL);
  for (uint8_t i = 0; i < 8; i++) {
    m_apLed[i] = gslc_ElemCreateBox(&m_gui, E_LED0 + i, E_PG_MAIN,
                   (gslc_tsRect){ (int16_t)(706 + i * 36), 175, 31, 31 });
    gslc_ElemSetDrawFunc(&m_gui, m_apLed[i], &CbDrawLed);
  }

  // 8 input LEDs (In0..In7), yellow glass symbols, just below the output row
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_LBL_IN, E_PG_MAIN, (gslc_tsRect){ 700, 215, 300, 36 },
                                (char*)"Inputs 1-8", 0, E_FONT_TAB);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GEVA_BLUE_COL, GEVA_BLUE_COL, GEVA_BLUE_COL);
  for (uint8_t i = 0; i < 8; i++) {
    m_apLedIn[i] = gslc_ElemCreateBox(&m_gui, E_LEDI0 + i, E_PG_MAIN,
                     (gslc_tsRect){ (int16_t)(706 + i * 36), 255, 31, 31 });
    gslc_ElemSetDrawFunc(&m_gui, m_apLedIn[i], &CbDrawLedIn);
  }

  // Buttons (glass image buttons, 251x70). The row is spaced so OUT0 lines up
  // under the FONTS button (both at x = 720).
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_LED_ON, E_PG_MAIN, (gslc_tsRect){ 60, 330, 251, 70 },
                                   (char*)"LED ON", 0, E_FONT_TXT, &CbBtn);
  gslc_ElemSetDrawFunc(&m_gui, pElemRef, &CbDrawBtnBig);
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_LED_OFF, E_PG_MAIN, (gslc_tsRect){ 390, 330, 251, 70 },
                                   (char*)"LED OFF", 0, E_FONT_TXT, &CbBtn);
  gslc_ElemSetDrawFunc(&m_gui, pElemRef, &CbDrawBtnBig);
  m_pBtnOut0 = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_OUT0, E_PG_MAIN, (gslc_tsRect){ 720, 330, 251, 70 },
                                     m_acBtnOut0, sizeof(m_acBtnOut0), E_FONT_TXT, &CbBtn);
  gslc_ElemSetDrawFunc(&m_gui, m_pBtnOut0, &CbDrawBtnBig);

  // "Analog out:" label + value text on the same row. Height >= the 32pt glyph
  // height (45 px) so the blue fill fully clears descenders (e.g. the "g" in
  // "Light") when the value text changes length.
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_LBL_DAC, E_PG_MAIN, (gslc_tsRect){ 60, 450, 360, 50 },
                                (char*)"Analog out:", 0, E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GEVA_BLUE_COL, GEVA_BLUE_COL, GEVA_BLUE_COL);

  m_pTxtDac = gslc_ElemCreateTxt(&m_gui, E_TXT_DAC, E_PG_MAIN, (gslc_tsRect){ 430, 450, 280, 50 },
                                 m_acTxtDac, sizeof(m_acTxtDac), E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, m_pTxtDac, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, m_pTxtDac, GEVA_BLUE_COL, GEVA_BLUE_COL, GEVA_BLUE_COL);

  // Slider (custom glass round thumb): drives the DAC (analog) or selects 1 of 8 lights (digital)
  m_pSlider = gslc_ElemXSliderCreate(&m_gui, E_SLIDER_DAC, E_PG_MAIN, &m_sSliderDac,
                                     (gslc_tsRect){ 60, 500, 640, 50 }, 0, 100, m_nDacPos, 31, false);
  gslc_ElemXSliderSetPosFunc(&m_gui, m_pSlider, &CbSlider);
  gslc_ElemSetDrawFunc(&m_gui, m_pSlider, &CbDrawSlider);

  // ANALOG / DIGITAL mode toggle, to the right of the slider (aligned under FONTS/OUT0)
  m_pBtnMode = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_MODE, E_PG_MAIN, (gslc_tsRect){ 720, 490, 251, 70 },
                                     m_acBtnMode, sizeof(m_acBtnMode), E_FONT_TXT, &CbBtn);
  gslc_ElemSetDrawFunc(&m_gui, m_pBtnMode, &CbDrawBtnBig);

  // -----------------------------------
  // Build the welcome splash and the font-gallery page
  InitPageWelcome();
  InitPageFonts();

  // -----------------------------------
  // Start on the 5-second welcome splash; loop() switches to the main page
  gslc_SetPageCur(&m_gui, E_PG_WELCOME);
  m_tWelcome.reset();
  m_tSec.reset();   // start the 1-second tick cleanly
}

// -----------------------------------------------------------------------------
// Welcome / splash page (shown for 5 s at boot)
// -----------------------------------------------------------------------------
void InitPageWelcome(void)
{
  gslc_tsElemRef* pRef;

  gslc_PageAdd(&m_gui, E_PG_WELCOME, m_asElemWelcome, MAX_ELEM_PG_WELCOME,
               m_asElemRefWelcome, MAX_ELEM_PG_WELCOME);

  // Full-screen dark box: darkens the blue background for the splash. Added
  // first so the texts below are drawn on top of it.
  pRef = gslc_ElemCreateBox(&m_gui, E_W_BG, E_PG_WELCOME, (gslc_tsRect){ 0, 0, 1024, 600 });
  gslc_ElemSetCol(&m_gui, pRef, GEVA_DARK_COL, GEVA_DARK_COL, GEVA_DARK_COL);
  gslc_ElemSetFillEn(&m_gui, pRef, true);

  pRef = gslc_ElemCreateTxt(&m_gui, E_W_TITLE, E_PG_WELCOME, (gslc_tsRect){ 0, 150, 1024, 70 },
                            (char*)"GEVA elettronica", 0, E_FONT_WBIG);
  gslc_ElemSetTxtCol(&m_gui, pRef, GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(&m_gui, pRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui, pRef, false);

  pRef = gslc_ElemCreateTxt(&m_gui, E_W_SUB, E_PG_WELCOME, (gslc_tsRect){ 0, 250, 1024, 50 },
                            (char*)"gevino TFT", 0, E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui, pRef, GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(&m_gui, pRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui, pRef, false);

  pRef = gslc_ElemCreateTxt(&m_gui, E_W_URL, E_PG_WELCOME, (gslc_tsRect){ 0, 330, 1024, 40 },
                            (char*)"shop.gevaelettronica.it", 0, E_FONT_TAB);
  gslc_ElemSetTxtCol(&m_gui, pRef, GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(&m_gui, pRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui, pRef, false);

  pRef = gslc_ElemCreateTxt(&m_gui, E_W_THEME, E_PG_WELCOME, (gslc_tsRect){ 0, 520, 1024, 40 },
                            (char*)"glass theme", 0, E_FONT_ITAL);
  gslc_ElemSetTxtCol(&m_gui, pRef, GSLC_COL_WHITE);
  gslc_ElemSetTxtAlign(&m_gui, pRef, GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui, pRef, false);
}

// -----------------------------------------------------------------------------
// Build the font-gallery page: a tab bar (one tab per font family) + a Back
// button + 9 white sample lines (one per size). Switching tab re-points the
// sample font slots to the selected family (see ShowFamily).
// -----------------------------------------------------------------------------
void InitPageFonts(void)
{
  gslc_tsElemRef* pElemRef;

  gslc_PageAdd(&m_gui, E_PG_FONTS, m_asElemFonts, MAX_ELEM_PG_FONTS,
               m_asElemRefFonts, MAX_ELEM_PG_FONTS);

  // Family name header
  m_pFamName = gslc_ElemCreateTxt(&m_gui, E_FS_NAME, E_PG_FONTS, (gslc_tsRect){ 6, 64, 1012, 40 },
                                  m_acFamName, sizeof(m_acFamName), E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui, m_pFamName, GSLC_COL_WHITE);
  gslc_ElemSetFillEn(&m_gui, m_pFamName, false);

  // Tab bar: 7 family tabs + Back (glass image buttons, 105x55)
  for (uint8_t i = 0; i < 7; i++) {
    pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_TAB_SANS + i, E_PG_FONTS,
                 (gslc_tsRect){ (int16_t)(80 + i * 117), 8, 105, 55 },
                 (char*)TAB_LABELS[i], 0, E_FONT_TAB, &CbTab);
    gslc_ElemSetDrawFunc(&m_gui, pElemRef, &CbDrawBtnTab);
  }
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_BTN_BACK, E_PG_FONTS,
               (gslc_tsRect){ (int16_t)(80 + 7 * 117), 8, 105, 55 },
               (char*)"Back", 0, E_FONT_TAB, &CbTab);
  gslc_ElemSetDrawFunc(&m_gui, pElemRef, &CbDrawBtnTab);

  // 9 sample lines (white, transparent fill), each bound to a reusable font slot
  for (uint8_t i = 0; i < 9; i++) {
    m_apSample[i] = gslc_ElemCreateTxt(&m_gui, E_FS_S0 + i, E_PG_FONTS,
                      (gslc_tsRect){ FS_X[i], FS_Y[i], FS_W[i], FS_H[i] },
                      (char*)FS_SAMPLE[i], 0, E_FONT_FS0 + i);
    gslc_ElemSetTxtCol(&m_gui, m_apSample[i], GSLC_COL_WHITE);
    gslc_ElemSetFillEn(&m_gui, m_apSample[i], false);
  }
}

// -----------------------------------------------------------------------------
// loop()
// -----------------------------------------------------------------------------
void loop()
{
  // Service the GUI (drawing + touch)
  gslc_Update(&m_gui);

  // Dismiss the welcome splash after 5 s and show the main page
  if (m_bWelcome && m_tWelcome.stato()) {
    m_bWelcome = false;
    gslc_SetPageCur(&m_gui, E_PG_MAIN);
  }

  // Refresh the GEVINO shift-register I/O (opto inputs/outputs).
  // In0..In3 and Out0 are direct GPIO and do not strictly need this, but
  // calling it keeps Out1..Out7 / In4..In11 live for richer programs.
  updateIO();

  // Mirror the live input states on the yellow LED row: redraw only on change.
  static uint8_t nInPrev = 0xFF;
  uint8_t nInNow = 0;
  for (uint8_t i = 0; i < 8; i++) { if (inState(i)) nInNow |= (uint8_t)(1 << i); }
  if (nInNow != nInPrev) { nInPrev = nInNow; refreshLedsIn(); }

  // Digital-light show (DIGITAL mode only): while the slider is held the 8 lights
  // run (chase); on release they oscillate (-1,-2,+1,+2) and settle on the
  // selected light.
  if (m_eMode == MODE_DIGITAL) {
    bool bHeld = gslc_ElemGetGlow(&m_gui, m_pSlider);
    if (bHeld) {
      m_bSettle = false;
      // Held still long enough -> run the lights (while moving, CbSlider keeps
      // lighting the position and resetting m_tStill, so the chase stays off).
      if (m_tStill.stato() && m_tChase.stato()) {
        m_tChase.reset();
        m_nChaseIdx = (uint8_t)((m_nChaseIdx + 1) & 0x07);
        setLight(m_nChaseIdx);
      }
    } else if (m_bHeldPrev) {                        // just released -> start oscillation
      m_nLightSel   = posToIdx(m_nDacPos);
      m_bSettle     = true;
      m_nSettleStep = 0;
      m_tSettle.reset();
      setLight(clampIdx((int16_t)m_nLightSel + SETTLE_OFFS[0]));
    } else if (m_bSettle && m_tSettle.stato()) {     // advance the oscillation
      m_tSettle.reset();
      m_nSettleStep++;
      if (m_nSettleStep < 5) {
        setLight(clampIdx((int16_t)m_nLightSel + SETTLE_OFFS[m_nSettleStep]));
      } else {
        m_bSettle = false;
        setLight(m_nLightSel);                       // end on the correct light
      }
    }
    m_bHeldPrev = bHeld;
  }

  // Once per second: update the dynamic text fields
  if (m_tSec.stato()) {
    m_tSec.reset();

    char acTxt[20];
    snprintf(acTxt, sizeof(acTxt), "Counter: %lu", (unsigned long)(++m_nCount));
    gslc_ElemSetTxtStr(&m_gui, m_pTxtCount, acTxt);

    char acIn[24];
    snprintf(acIn, sizeof(acIn), "Input In0: %s", In0 ? "ON" : "OFF");
    gslc_ElemSetTxtStr(&m_gui, m_pTxtIn0, acIn);
  }
}
