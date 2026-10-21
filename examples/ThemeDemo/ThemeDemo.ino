//
// GEVINO TFT — Animated theme demo (GLASS / FLUO / ALU)
// GEVA Elettronica - https://www.gevaelettronica.it
//
//   One All_76_Pic_65K.bin in the display flash holds three themes. The sketch
//   changes look at run time by pointing at another block of indices: nothing
//   is reprogrammed.
//
//   Three real GUIslice pages, one per theme, each with its own background.
//   Everything animates on its own: running lights, a dimmer that sweeps, a
//   level bar, temperature, a blinking alarm lamp, a toggle that flips, symbols
//   that cycle, a clock. Touching the dimmer suspends its automation.
//
//   Symbols are not images: they come from two symbol FONTS in font.bin
//   (symbols_32pt, symbols_48pt), so they are transparent, take any colour at
//   run time and sit on top of any button of any theme.
//
//   The three buttons at the bottom switch page, i.e. theme.
//
//   Hardware:
//     - GEVINO TFT, Arduino Zero compatible (SAMD21)
//     - 1024x600, RAiO RA8876 (SPI, TFT_CS = 42)
//     - capacitive touch: FT5206 on the 7", GT9271 on the 10" (auto-detected)
//
// =============================================================================
// PREREQUISITES
//   1. Board:    "Arduino Zero (Native USB Port)" (Arduino SAMD core).
//   2. Libraries: GEVINO TFT (gevino_tft_io.h, gevino_tft_touch.h +
//                gevinoTFT_RA8876) and FT5206 (used by GUIslice's 7" driver).
//                The GEVINO TFT library needs the fonts for this orientation in
//                src/ (font.h + fontExt.h + fontSymbols.h from src/Horizzontal/
//                for 1024x600), and a font.bin that includes the symbol fonts
//                (ENDFLASH 3837052) programmed into the display flash.
//   3. Config:   in GUIslice/src/GUIslice_config.h this line must be enabled:
//                  #include "../configs/ard-shld-gevino_tft.h"
//   4. Flash:    put font.bin and All_76_Pic_65K.bin on a microSD and insert
//                it. The library does the rest: RA8876::init() ends with
//                copySDtoFlash(), which rewrites font.bin / All_76_Pic_65K.bin
//                into the display flash when it finds them. With no card, or
//                no such files, it does nothing, so it is safe on every boot.
//                Together they are ~8 MB, so the first boot with the card in is
//                slow; later ones are not.
//
// TEXT AND TRANSPARENCY
//   Every string here is drawn with the GEVA flash fonts (setFont + print),
//   never with the RA8876 internal ROM font. The BTE expands those glyphs with
//   colour expansion + chroma keying, so they land on top of the button images
//   with a transparent background and a colour chosen at run time.
//
//   The images themselves have no transparency: pic_b() -> DMA_24bit() is a raw
//   copy that writes every pixel. That is why symbols are a font and not
//   images: printed like text, they get the same transparent background.
// =============================================================================

#include "gevino_tft_io.h"
#include "All_76_Pic_65K.h"      // the three themes: glass_*, fluo_*, alu_*
#include "GUIslice.h"
#include "GUIslice_drv.h"
#include "elem/XSlider.h"
#include "elem/XProgress.h"
#include "elem/XCheckbox.h"
#include "elem/XTogglebtn.h"
#include "fontExt.h"
#include "Horizzontal/font.h"
#include "fontSymbols.h"         // SYM_POWER, SYM_BULB, ... for symbols_32pt/48pt

// Capacitive touch (7" FT5206 / 10" GT9271, auto-detected) + GUIslice bridge.
// The bridge needs the gslc_ types, so it comes after GUIslice.h.
#include "gevino_tft_touch.h"
#include "gevino_tft_touch_gslc.h"

extern "C" RA8876* gslc_GevinoDisp(void);

// ================================================ touch: 7" and 10" ==========
//
// The board ships with two different capacitive controllers (FT5206 on the 7",
// GT9271 on the 10"). The GEVINO TFT library detects which one is fitted and
// gevino_touch_gui_update() feeds it to GUIslice: on a 7" board that is a no-op
// (GUIslice polls the FT5206 itself through DRV_TOUCH_ADA_FT5206), on a 10" one
// the library reads the GT9271 and injects the points.

// ================================================== the three themes =========

#define NO_IMG (-1)
#define N_THEMES 3
#define N_LED    6      // running lights
#define N_SYM    6      // symbols in the carousel

// The symbols are one font shared by every theme: each theme only picks the
// colours it prints them in.
static const char* const SYMS[N_SYM] = {
  SYM_POWER, SYM_BULB, SYM_SNOWFLAKE, SYM_FAN, SYM_BELL, SYM_WIFI
};

// How a theme prints a symbol. With 'shaded' the glyph goes down twice: first
// 'shade' one pixel lower, then 'col' on top. On ALU that is the engraved
// look of the old symbol images (light edge under a dark stroke on the metal,
// dark edge under a light stroke on the background).
typedef struct {
  gslc_tsColor col;
  gslc_tsColor shade;
  bool         shaded;
} SymInk;

typedef struct {
  const char*  name;
  const char*  caption;
  gslc_tsColor colBg;
  gslc_tsColor colText;
  gslc_tsColor colDim;        // secondary text
  gslc_tsColor colAccent;     // level bar, values
  int16_t rect251Rel, rect251Prs;
  int16_t rect146Rel, rect146Prs;
  int16_t round70Rel, round70Prs;
  int16_t ledOn[N_LED];
  int16_t ledOff;
  int16_t checkOff, checkOn;   // NO_IMG -> the widget is not created
  int16_t swOff,    swOn;      // NO_IMG -> the widget is not created
  int16_t knob;
  SymInk  symOnBg[N_SYM];      // carousel, on the screen background
  SymInk  symOnBtn;            // on the round button, over its artwork
} Theme;

static const Theme THEMES[N_THEMES] = {
  // -------------------------------------------------------------- GLASS ---
  // No check and no toggle yet: that artwork is still to be drawn, so those
  // widgets are simply not created on this page.
  { "GLASS", "Glossy blue glass",
    { 0x23, 0x90, 0xEC }, { 0xFF, 0xFF, 0xFF },
    { 0xC8, 0xE4, 0xFA }, { 0xFF, 0xFF, 0xFF },
    glass_rect_251_70_released, glass_rect_251_70_pressed,
    glass_rect_146_55_released, glass_rect_146_55_pressed,
    glass_round_70_released,    glass_round_70_pressed,
    { glass_round_31_led_green,  glass_round_31_led_yellow,
      glass_round_31_led_orange, glass_round_31_led_red,
      glass_round_31_led_yellow, glass_round_31_led_green },
    glass_round_31_led_off,
    NO_IMG, NO_IMG,  NO_IMG, NO_IMG,  glass_round_31_released,
    { { { 0xFF, 0xFF, 0xFF }, { 0, 0, 0 }, false },
      { { 0xFF, 0xE6, 0x4D }, { 0, 0, 0 }, false },
      { { 0xFF, 0xFF, 0xFF }, { 0, 0, 0 }, false },
      { { 0xFF, 0xFF, 0xFF }, { 0, 0, 0 }, false },
      { { 0xFF, 0xA0, 0x40 }, { 0, 0, 0 }, false },
      { { 0xFF, 0xFF, 0xFF }, { 0, 0, 0 }, false } },
    { { 0xFF, 0xFF, 0xFF }, { 0, 0, 0 }, false }
  },
  // --------------------------------------------------------------- FLUO ---
  { "FLUO", "Pure neon on pure black",
    { 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF },
    { 0x78, 0x82, 0x8C }, { 0xFF, 0x00, 0xFF },
    fluo_rect_251_70_magenta_released, fluo_rect_251_70_magenta_pressed,
    fluo_rect_146_55_magenta_released, fluo_rect_146_55_magenta_pressed,
    fluo_round_70_magenta_released,    fluo_round_70_magenta_pressed,
    { fluo_led_31_green_on,   fluo_led_31_yellow_on,
      fluo_led_31_red_on,     fluo_led_31_magenta_on,
      fluo_led_31_blue_on,    fluo_led_31_cyan_on },
    fluo_led_31_white_off,
    fluo_check_36_magenta_off, fluo_check_36_magenta_on,
    fluo_switch_100_48_magenta_off, fluo_switch_100_48_magenta_on,
    fluo_knob_40_magenta,
    // pure colours, as the neon artwork
    { { { 0xFF, 0x00, 0xFF }, { 0, 0, 0 }, false },
      { { 0xFF, 0xFF, 0x00 }, { 0, 0, 0 }, false },
      { { 0x00, 0xFF, 0xFF }, { 0, 0, 0 }, false },
      { { 0x00, 0xFF, 0x00 }, { 0, 0, 0 }, false },
      { { 0xFF, 0x00, 0x00 }, { 0, 0, 0 }, false },
      { { 0xFF, 0xFF, 0xFF }, { 0, 0, 0 }, false } },
    { { 0xFF, 0xFF, 0xFF }, { 0, 0, 0 }, false }
  },
  // ---------------------------------------------------------------- ALU ---
  { "ALU", "Brushed aluminium, relief and shadow",
    { 0x1A, 0x1D, 0x21 }, { 0xEC, 0xF0, 0xF5 },
    { 0x86, 0x8E, 0x99 }, { 0x3A, 0x8C, 0xD6 },
    alu_rect_251_70_alu_released, alu_rect_251_70_alu_pressed,
    alu_rect_146_55_alu_released, alu_rect_146_55_alu_pressed,
    alu_round_70_alu_released,    alu_round_70_alu_pressed,
    { alu_led_31_green_on, alu_led_31_amber_on,
      alu_led_31_red_on,   alu_led_31_blue_on,
      alu_led_31_white_on, alu_led_31_green_on },
    alu_led_31_white_off,
    alu_check_36_blue_off, alu_check_36_blue_on,
    alu_switch_100_48_blue_off, alu_switch_100_48_blue_on,
    alu_knob_40_alu,
    // on the dark background: light stroke over a dark edge; power in blue
    { { { 0x29, 0x8C, 0xFF }, { 0x0F, 0x12, 0x14 }, true },
      { { 0xE0, 0xE8, 0xF2 }, { 0x0F, 0x12, 0x14 }, true },
      { { 0xE0, 0xE8, 0xF2 }, { 0x0F, 0x12, 0x14 }, true },
      { { 0xE0, 0xE8, 0xF2 }, { 0x0F, 0x12, 0x14 }, true },
      { { 0xE0, 0xE8, 0xF2 }, { 0x0F, 0x12, 0x14 }, true },
      { { 0xE0, 0xE8, 0xF2 }, { 0x0F, 0x12, 0x14 }, true } },
    // on the light metal: engraved, dark stroke over a light edge
    { { 0x1C, 0x1F, 0x24 }, { 0xC7, 0xD1, 0xDB }, true }
  }
};

// ===================================================== GUIslice plumbing =====

// An element id carries its page: id = theme*100 + slot.
#define ID(theme, slot)  ((theme) * 100 + (slot))
#define ID_THEME(id)     ((id) / 100)
#define ID_SLOT(id)      ((id) % 100)

enum { S_THEME0 = 0, S_THEME1, S_THEME2,     // selector at the bottom
       S_BTN_LIGHT, S_BTN_CLIMATE, S_BTN_ROUND,
       S_SLIDER, S_PROGRESS, S_CHECK, S_TOGGLE,
       S_HEADER, S_PANEL, S_MAX };

#define MAX_PAGE  N_THEMES
#define MAX_ELEM  S_MAX
#define MAX_FONT  1                       // GUIslice needs one slot; we draw text ourselves
enum { E_FONT_DUMMY };

gslc_tsGui      m_gui;
gslc_tsDriver   m_drv;
gslc_tsFont     m_asFont[MAX_FONT];
gslc_tsPage     m_asPage[MAX_PAGE];
gslc_tsElem     m_asElem[N_THEMES][MAX_ELEM];
gslc_tsElemRef  m_asElemRef[N_THEMES][MAX_ELEM];

gslc_tsXSlider    m_sSlider[N_THEMES];
gslc_tsXProgress  m_sProgress[N_THEMES];
gslc_tsXCheckbox  m_sCheck[N_THEMES];
gslc_tsXTogglebtn m_sToggle[N_THEMES];

gslc_tsElemRef* m_pRef[N_THEMES][MAX_ELEM];   // NULL = not present on that page

char m_acTheme[N_THEMES][8];
char m_acLight[N_THEMES][10];
char m_acClimate[N_THEMES][10];
char m_acRound[N_THEMES][4];

// -------------------------------------------------------------- demo state --

static uint8_t  g_theme    = 1;     // current page/theme: starts on FLUO
static uint8_t  g_ledPos   = 0;
static uint8_t  g_symPos   = 0;
static bool     g_alarm    = false;
static bool     g_lightOn  = false;
static int16_t  g_dimmer   = 40;    // 0..100
static int8_t   g_dimDir   = +2;
static int16_t  g_level    = 0;     // 0..100
static int8_t   g_levDir   = +3;
static int16_t  g_degrees  = 205;   // tenths: 20.5
static int8_t   g_degDir   = +1;
static uint32_t g_manualUntil = 0;
static uint32_t g_seconds  = 0;

static uint32_t tLed = 0, tSym = 0, tAnim = 0, tSec = 0, tSw = 0;

#define TH   (THEMES[g_theme])

// ------------------------------------------------------------------ helpers --

static uint16_t Col565(gslc_tsColor c) { return RGB565(c.r, c.g, c.b); }

static void Blit(int16_t nImg, int16_t x, int16_t y)
{
  if (nImg == NO_IMG) return;
  gslc_GevinoDisp()->pic_b(&BINARY_INFO[nImg], x, y);
}

// All text goes through here: always a GEVA flash font, never the internal
// ROM font of the controller. setTextColor() with one argument selects the
// BTE mode with chroma keying, so the glyph background stays transparent.
static void Text(const char* str, int16_t x, int16_t y, uint16_t col,
                 const FONT_INFO* pFont)
{
  if (!str || !str[0]) return;
  RA8876* tft = gslc_GevinoDisp();
  tft->setFont(pFont);
  tft->setTextColor(col);
  tft->setCursor(x, y);
  tft->print(str);
}

static void TextCentred(const char* str, int16_t x, int16_t y,
                        int16_t w, int16_t h, uint16_t col,
                        const FONT_INFO* pFont)
{
  if (!str || !str[0]) return;
  RA8876*  tft = gslc_GevinoDisp();
  int16_t  bx, by;
  uint16_t bw, bh;
  tft->setFont(pFont);
  tft->setTextColor(col);
  tft->getTextBounds(str, 0, 0, &bx, &by, &bw, &bh);
  tft->setCursor(x + (w - (int16_t)bw) / 2, y + (h - (int16_t)bh) / 2);
  tft->print(str);
}

// A symbol from the symbol font. The glyph is square and as large as the
// font, so (x, y) is its top-left corner, exactly like an image. Transparent
// like any GEVA font: what is underneath shows through.
// Plain parameters, not a SymInk*: see ClearArea() below for why.
static void Symbol(const char* sym, int16_t x, int16_t y, const FONT_INFO* pFont,
                   uint16_t col, uint16_t shade, bool shaded)
{
  RA8876* tft = gslc_GevinoDisp();
  tft->setFont(pFont);
  if (shaded) {
    tft->setTextColor(shade);
    tft->setCursor(x, y + 1);
    tft->print(sym);
  }
  tft->setTextColor(col);
  tft->setCursor(x, y);
  tft->print(sym);
}

// Clear a rectangle with the theme background.
//
// The colour is passed as a plain uint16_t on purpose. The Arduino builder
// auto-generates function prototypes and inserts them just before the FIRST
// function of the sketch, which sits well above the "Theme" typedef, so any
// function taking a "Theme*" PARAMETER fails to compile with
// "'Theme' does not name a type" even though the order inside the file is
// correct. Local variables of type Theme* are fine; parameters are not.
//
// fillRect on the RA8876 takes TWO POINTS (x1,y1,x2,y2), not x,y,w,h.
static void ClearArea(uint16_t colBg, int16_t x, int16_t y, int16_t w, int16_t h)
{
  gslc_GevinoDisp()->fillRect(x, y, x + w, y + h, colBg);
}

static void Dirty(uint8_t theme, uint8_t slot)
{
  if (m_pRef[theme][slot]) {
    gslc_ElemSetRedraw(&m_gui, m_pRef[theme][slot], GSLC_REDRAW_INC);
  }
}

// ------------------------------------------------------------ themed draws --

// The three selector buttons always use the artwork of the theme currently on
// screen, so the bottom bar stays coherent with the page. The active one is
// shown pressed.
static bool CbDrawThemeBtn(void* pvGui, void* pvElemRef, gslc_teRedrawType eR)
{
  (void)eR;
  gslc_tsGui*     pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pEl  = gslc_GetElemFromRef(pGui, pRef);
  const Theme*    pT   = &TH;                 // artwork of the displayed theme
  uint8_t i = ID_SLOT(pEl->nId) - S_THEME0;
  if (i >= N_THEMES) i = 0;

  bool bActive = (i == g_theme) || gslc_ElemGetGlow(pGui, pRef);
  Blit(bActive ? pT->rect146Prs : pT->rect146Rel, pEl->rElem.x, pEl->rElem.y);
  TextCentred(pEl->pStrBuf, pEl->rElem.x, pEl->rElem.y, 146, 55,
              Col565(pT->colText), &notoSans_20pt);
  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);
  return true;
}

static bool CbDrawBtn251(void* pvGui, void* pvElemRef, gslc_teRedrawType eR)
{
  (void)eR;
  gslc_tsGui*     pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pEl  = gslc_GetElemFromRef(pGui, pRef);
  const Theme*    pT   = &THEMES[ID_THEME(pEl->nId)];

  bool bPressed = gslc_ElemGetGlow(pGui, pRef);
  if (ID_SLOT(pEl->nId) == S_BTN_LIGHT) bPressed = bPressed || g_lightOn;

  Blit(bPressed ? pT->rect251Prs : pT->rect251Rel, pEl->rElem.x, pEl->rElem.y);
  TextCentred(pEl->pStrBuf, pEl->rElem.x, pEl->rElem.y, 251, 70,
              Col565(pT->colText), &notoSans_32pt);
  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);
  return true;
}

static bool CbDrawBtnRound(void* pvGui, void* pvElemRef, gslc_teRedrawType eR)
{
  (void)eR;
  gslc_tsGui*     pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pEl  = gslc_GetElemFromRef(pGui, pRef);
  const Theme*    pT   = &THEMES[ID_THEME(pEl->nId)];

  Blit(gslc_ElemGetGlow(pGui, pRef) ? pT->round70Prs : pT->round70Rel,
       pEl->rElem.x, pEl->rElem.y);
  // a 48 px symbol right on top of the button artwork: (70 - 48) / 2 = 11
  Symbol(SYM_NEXT_TRACK, pEl->rElem.x + 11, pEl->rElem.y + 11, &symbols_48pt,
         Col565(pT->symOnBtn.col), Col565(pT->symOnBtn.shade),
         pT->symOnBtn.shaded);
  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);
  return true;
}

// Dimmer: drawn track plus the theme's knob. XSlider still owns the position
// and the drag handling; only the painting is overridden.
static bool CbDrawSlider(void* pvGui, void* pvElemRef, gslc_teRedrawType eR)
{
  (void)eR;
  gslc_tsGui*     pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pEl  = gslc_GetElemFromRef(pGui, pRef);
  const Theme*    pT   = &THEMES[ID_THEME(pEl->nId)];
  RA8876*         tft  = gslc_GevinoDisp();

  const int16_t kThumb = 40;
  int16_t x = pEl->rElem.x, y = pEl->rElem.y;
  int16_t w = pEl->rElem.w, h = pEl->rElem.h;
  int16_t pos = gslc_ElemXSliderGetPos(pGui, pRef);      // 0..100

  ClearArea(Col565(pT->colBg), x, y, w, h);
  int16_t yTrack = y + h / 2 - 2;
  int16_t xa = x + kThumb / 2, xb = x + w - kThumb / 2;
  tft->fillRect(xa, yTrack, xb, yTrack + 4, Col565(pT->colDim));
  int16_t xCur = xa + (int32_t)(xb - xa) * pos / 100;
  tft->fillRect(xa, yTrack, xCur, yTrack + 4, Col565(pT->colAccent));
  Blit(pT->knob, xCur - kThumb / 2, y + (h - kThumb) / 2);

  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);
  return true;
}

static bool CbDrawCheck(void* pvGui, void* pvElemRef, gslc_teRedrawType eR)
{
  (void)eR;
  gslc_tsGui*     pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pEl  = gslc_GetElemFromRef(pGui, pRef);
  const Theme*    pT   = &THEMES[ID_THEME(pEl->nId)];

  Blit(gslc_ElemXCheckboxGetState(pGui, pRef) ? pT->checkOn : pT->checkOff,
       pEl->rElem.x, pEl->rElem.y);
  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);
  return true;
}

static bool CbDrawToggle(void* pvGui, void* pvElemRef, gslc_teRedrawType eR)
{
  (void)eR;
  gslc_tsGui*     pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pEl  = gslc_GetElemFromRef(pGui, pRef);
  const Theme*    pT   = &THEMES[ID_THEME(pEl->nId)];

  Blit(gslc_ElemXTogglebtnGetState(pGui, pRef) ? pT->swOn : pT->swOff,
       pEl->rElem.x, pEl->rElem.y);
  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);
  return true;
}

// Header: theme name, caption, clock and temperature. All GEVA fonts.
static bool CbDrawHeader(void* pvGui, void* pvElemRef, gslc_teRedrawType eR)
{
  (void)eR;
  gslc_tsGui*     pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pEl  = gslc_GetElemFromRef(pGui, pRef);
  const Theme*    pT   = &THEMES[ID_THEME(pEl->nId)];
  int16_t x0 = pEl->rElem.x, y0 = pEl->rElem.y;
  char ac[16];

  ClearArea(Col565(pT->colBg), x0, y0, pEl->rElem.w, pEl->rElem.h);

  Text(pT->name,    x0, y0,      Col565(pT->colText),  &notoSans_32pt);
  Text(pT->caption, x0, y0 + 40, Col565(pT->colDim),   &notoSans_12pt);

  snprintf(ac, sizeof(ac), "%d.%d C", g_degrees / 10, g_degrees % 10);
  Text(ac, x0 + 520, y0 + 4, Col565(pT->colAccent), &notoSans_32pt);

  snprintf(ac, sizeof(ac), "%02u:%02u",
           (unsigned)((g_seconds / 60) % 100), (unsigned)(g_seconds % 60));
  Text(ac, x0 + 800, y0 + 4, Col565(pT->colText), &notoSans_32pt);

  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);
  return true;
}

// Panel: everything that animates by itself and is not touchable.
// One element instead of twenty, repainted whenever something changes.
static bool CbDrawPanel(void* pvGui, void* pvElemRef, gslc_teRedrawType eR)
{
  (void)eR;
  gslc_tsGui*     pGui = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pEl  = gslc_GetElemFromRef(pGui, pRef);
  const Theme*    pT   = &THEMES[ID_THEME(pEl->nId)];
  int16_t x0 = pEl->rElem.x, y0 = pEl->rElem.y;
  uint16_t cTxt = Col565(pT->colText), cDim = Col565(pT->colDim);

  ClearArea(Col565(pT->colBg), x0, y0, pEl->rElem.w, pEl->rElem.h);

  // --- running lights -------------------------------------------------------
  Text("RUNNING LIGHTS", x0, y0, cDim, &notoSans_12pt);
  for (uint8_t i = 0; i < N_LED; i++) {
    Blit((i == g_ledPos) ? pT->ledOn[i] : pT->ledOff, x0 + i * 42, y0 + 26);
  }

  // --- blinking alarm lamp --------------------------------------------------
  Blit(g_alarm ? pT->ledOn[2] : pT->ledOff, x0 + 300, y0 + 26);
  Text(g_alarm ? "ALARM" : "normal", x0 + 340, y0 + 32,
       g_alarm ? cTxt : cDim, &notoSans_12pt);

  // --- symbol carousel: the symbol font, in this theme's colours ------------
  Text("SYMBOLS", x0 + 520, y0, cDim, &notoSans_12pt);
  for (uint8_t i = 0; i < 3; i++) {
    uint8_t k = (g_symPos + i) % N_SYM;
    Symbol(SYMS[k], x0 + 520 + i * 60, y0 + 24, &symbols_48pt,
           Col565(pT->symOnBg[k].col), Col565(pT->symOnBg[k].shade),
           pT->symOnBg[k].shaded);
  }

  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);
  return true;
}

// ------------------------------------------------------------------- touch ---

static void GoToTheme(uint8_t next)
{
  if (next >= N_THEMES || next == g_theme) return;
  g_theme = next;
  gslc_SetBkgndColor(&m_gui, TH.colBg);      // the background belongs to the theme
  gslc_SetPageCur(&m_gui, next);
  gslc_PageRedrawSet(&m_gui, true);
}

static bool CbBtn(void* pvGui, void* pvElemRef, gslc_teTouch eTouch,
                  int16_t nX, int16_t nY)
{
  (void)pvGui; (void)nX; (void)nY;
  if (eTouch != GSLC_TOUCH_UP_IN) return true;

  gslc_tsElem* pEl = gslc_GetElemFromRef(&m_gui, (gslc_tsElemRef*)pvElemRef);
  switch (ID_SLOT(pEl->nId)) {
    case S_THEME0: GoToTheme(0); break;
    case S_THEME1: GoToTheme(1); break;
    case S_THEME2: GoToTheme(2); break;
    case S_BTN_LIGHT:
      g_lightOn = !g_lightOn;
      snprintf(m_acLight[g_theme], sizeof(m_acLight[0]),
               g_lightOn ? "LIGHT ON" : "LIGHT OFF");
      Dirty(g_theme, S_BTN_LIGHT);
      break;
    case S_BTN_CLIMATE:
      g_degDir = -g_degDir;
      break;
    case S_BTN_ROUND:
      g_ledPos = (g_ledPos + 1) % N_LED;
      Dirty(g_theme, S_PANEL);
      break;
    default: break;
  }
  return true;
}

// XTogglebtn calls this pointer WITHOUT checking it for NULL
// (XTogglebtn.c: "(*pTogglebtn->pfunctUser)(...)"), so passing NULL to
// gslc_ElemXTogglebtnCreate() crashes the board on the first press.
// An empty callback is all it takes.
static bool CbToggle(void* pvGui, void* pvElemRef, gslc_teTouch eTouch,
                     int16_t nX, int16_t nY)
{
  (void)pvGui; (void)pvElemRef; (void)eTouch; (void)nX; (void)nY;
  return true;
}

// The dimmer was dragged: suspend the automatic sweep for a while
static bool CbSliderPos(void* pvGui, void* pvElem, int16_t nPos)
{
  (void)pvGui; (void)pvElem;
  g_dimmer = nPos;
  g_manualUntil = millis() + 6000;
  return true;
}

// ================================================================== page ======

static void CreatePage(uint8_t t)
{
  const Theme* pT = &THEMES[t];
  gslc_tsElemRef* pRef;

  gslc_PageAdd(&m_gui, t, m_asElem[t], MAX_ELEM, m_asElemRef[t], MAX_ELEM);

  // ---- header (name, caption, temperature, clock) -------------------------
  pRef = gslc_ElemCreateBox(&m_gui, ID(t, S_HEADER), t,
           (gslc_tsRect){ 24, 16, 976, 60 });
  gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawHeader);
  m_pRef[t][S_HEADER] = pRef;

  // ---- two large buttons and a round one ----------------------------------
  snprintf(m_acLight[t], sizeof(m_acLight[0]), "LIGHT OFF");
  pRef = gslc_ElemCreateBtnTxt(&m_gui, ID(t, S_BTN_LIGHT), t,
           (gslc_tsRect){ 24, 90, 251, 70 },
           m_acLight[t], sizeof(m_acLight[0]), E_FONT_DUMMY, &CbBtn);
  gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawBtn251);
  m_pRef[t][S_BTN_LIGHT] = pRef;

  snprintf(m_acClimate[t], sizeof(m_acClimate[0]), "CLIMATE");
  pRef = gslc_ElemCreateBtnTxt(&m_gui, ID(t, S_BTN_CLIMATE), t,
           (gslc_tsRect){ 295, 90, 251, 70 },
           m_acClimate[t], sizeof(m_acClimate[0]), E_FONT_DUMMY, &CbBtn);
  gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawBtn251);
  m_pRef[t][S_BTN_CLIMATE] = pRef;

  // the label is not drawn: CbDrawBtnRound prints SYM_NEXT_TRACK instead
  snprintf(m_acRound[t], sizeof(m_acRound[0]), ">");
  pRef = gslc_ElemCreateBtnTxt(&m_gui, ID(t, S_BTN_ROUND), t,
           (gslc_tsRect){ 570, 90, 70, 70 },
           m_acRound[t], sizeof(m_acRound[0]), E_FONT_DUMMY, &CbBtn);
  gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawBtnRound);
  m_pRef[t][S_BTN_ROUND] = pRef;

  // ---- dimmer: a real XSlider, wearing the theme's knob --------------------
  pRef = gslc_ElemXSliderCreate(&m_gui, ID(t, S_SLIDER), t, &m_sSlider[t],
           (gslc_tsRect){ 24, 186, 520, 48 }, 0, 100, g_dimmer, 40, false);
  gslc_ElemXSliderSetPosFunc(&m_gui, pRef, &CbSliderPos);
  gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawSlider);
  m_pRef[t][S_SLIDER] = pRef;

  // ---- level bar: native XProgress, in the theme's colours -----------------
  pRef = gslc_ElemXProgressCreate(&m_gui, ID(t, S_PROGRESS), t, &m_sProgress[t],
           (gslc_tsRect){ 580, 198, 400, 24 }, 0, 100, g_level,
           pT->colAccent, false);
  gslc_ElemSetCol(&m_gui, pRef, pT->colDim, pT->colBg, pT->colBg);
  m_pRef[t][S_PROGRESS] = pRef;

  // ---- check and toggle: created only where the artwork exists -------------
  if (pT->checkOff != NO_IMG) {
    pRef = gslc_ElemXCheckboxCreate(&m_gui, ID(t, S_CHECK), t, &m_sCheck[t],
             (gslc_tsRect){ 24, 254, 36, 36 }, false,
             GSLCX_CHECKBOX_STYLE_X, pT->colAccent, false);
    gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawCheck);
    m_pRef[t][S_CHECK] = pRef;
  }

  if (pT->swOff != NO_IMG) {
    pRef = gslc_ElemXTogglebtnCreate(&m_gui, ID(t, S_TOGGLE), t, &m_sToggle[t],
             (gslc_tsRect){ 96, 250, 100, 48 },
             pT->colDim, pT->colAccent, pT->colText, true, false, &CbToggle);
    gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawToggle);
    m_pRef[t][S_TOGGLE] = pRef;
  }

  // ---- animated panel ------------------------------------------------------
  pRef = gslc_ElemCreateBox(&m_gui, ID(t, S_PANEL), t,
           (gslc_tsRect){ 24, 320, 976, 120 });
  gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawPanel);
  m_pRef[t][S_PANEL] = pRef;

  // ---- theme selector at the bottom ---------------------------------------
  for (uint8_t i = 0; i < N_THEMES; i++) {
    snprintf(m_acTheme[i], sizeof(m_acTheme[0]), "%s", THEMES[i].name);
    pRef = gslc_ElemCreateBtnTxt(&m_gui, ID(t, S_THEME0 + i), t,
             (gslc_tsRect){ (int16_t)(24 + i * 160), 500, 146, 55 },
             m_acTheme[i], sizeof(m_acTheme[0]), E_FONT_DUMMY, &CbBtn);
    gslc_ElemSetDrawFunc(&m_gui, pRef, &CbDrawThemeBtn);
    m_pRef[t][S_THEME0 + i] = pRef;
  }
}

// ================================================================== setup =====

void setup()
{
  SerialUSB.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);

  gevino_io_setup();

  // Which panel is fitted? Probe before gslc_Init(), which starts GUIslice's
  // FT5206 driver regardless.
  gevino_touch_setup();
  SerialUSB.print("Touch panel: "); SerialUSB.println(gevino_touch_name());

  // gslc_Init() runs RA8876::init(), which ends with copySDtoFlash(): with a
  // microSD holding All_76_Pic_65K.bin the flash is (re)written by itself.
  if (!gslc_Init(&m_gui, &m_drv, m_asPage, MAX_PAGE, m_asFont, MAX_FONT)) return;

  // One font slot is required by GUIslice even though every string in this
  // sketch is drawn by hand with a GEVA flash font.
  if (!gslc_FontSet(&m_gui, E_FONT_DUMMY, GSLC_FONTREF_PTR, &notoSans_20pt, 1)) return;

  for (uint8_t t = 0; t < N_THEMES; t++) CreatePage(t);

  gslc_SetBkgndColor(&m_gui, TH.colBg);
  gslc_SetPageCur(&m_gui, g_theme);
}

// =================================================================== loop =====

void loop()
{
  updateIO();

  // The 7" is polled by gslc_Update() itself; the 10" the library feeds in.
  gevino_touch_gui_update(&m_gui);

  uint32_t now = millis();

  // --- running lights -------------------------------------------------------
  if (now - tLed >= 130) {
    tLed = now;
    g_ledPos = (g_ledPos + 1) % N_LED;
    Dirty(g_theme, S_PANEL);
  }

  // --- symbol carousel ------------------------------------------------------
  if (now - tSym >= 900) {
    tSym = now;
    g_symPos = (g_symPos + 1) % N_SYM;
    Dirty(g_theme, S_PANEL);
  }

  // --- dimmer and level -----------------------------------------------------
  if (now - tAnim >= 60) {
    tAnim = now;

    if (now > g_manualUntil) {           // a touch suspends the sweep
      g_dimmer += g_dimDir;
      if (g_dimmer >= 100) { g_dimmer = 100; g_dimDir = -2; }
      if (g_dimmer <= 0)   { g_dimmer = 0;   g_dimDir = +2; }
      gslc_ElemXSliderSetPos(&m_gui, m_pRef[g_theme][S_SLIDER], g_dimmer);
    }

    g_level += g_levDir;
    if (g_level >= 100) { g_level = 100; g_levDir = -3; }
    if (g_level <= 0)   { g_level = 0;   g_levDir = +3; }
    gslc_ElemXProgressSetVal(&m_gui, m_pRef[g_theme][S_PROGRESS], g_level);
  }

  // --- clock, temperature, alarm -------------------------------------------
  if (now - tSec >= 1000) {
    tSec = now;
    g_seconds++;

    g_degrees += g_degDir;
    if (g_degrees >= 245) g_degDir = -1;
    if (g_degrees <= 185) g_degDir = +1;

    g_alarm = !g_alarm;
    Dirty(g_theme, S_HEADER);
    Dirty(g_theme, S_PANEL);
  }

  // --- the toggle and the check flip every 4 s ------------------------------
  // Both are absent on the GLASS page until its artwork exists, hence the
  // NULL checks: m_pRef stays NULL for widgets that were never created.
  if (now - tSw >= 4000) {
    tSw = now;
    if (m_pRef[g_theme][S_TOGGLE]) {
      gslc_ElemXTogglebtnSetState(&m_gui, m_pRef[g_theme][S_TOGGLE],
        !gslc_ElemXTogglebtnGetState(&m_gui, m_pRef[g_theme][S_TOGGLE]));
    }
    if (m_pRef[g_theme][S_CHECK]) {
      gslc_ElemXCheckboxSetState(&m_gui, m_pRef[g_theme][S_CHECK],
        !gslc_ElemXCheckboxGetState(&m_gui, m_pRef[g_theme][S_CHECK]));
    }
  }

  gslc_Update(&m_gui);
}
