#ifndef _GEVINO_TFT_TOUCH_GSLC_H_
#define _GEVINO_TFT_TOUCH_GSLC_H_

/*
  GEVINO TFT - GUIslice bridge for the capacitive touch (7" / 10")
  GEVA Elettronica - https://www.gevaelettronica.it

  GUIslice ships a touch driver for the FT5206 (7" panel) but knows nothing
  about the GT9271 (10"). This bridge feeds the GEVINO touch module
  (gevino_tft_touch.h) into GUIslice, so the same sketch runs on both panels:

    - 10" panel: the points are read here and injected with gslc_TrackTouch().
    -  7" panel: left to GUIslice's own DRV_TOUCH_ADA_FT5206 driver when the
      config selects it; if the config has no touch driver (DRV_TOUCH_NONE),
      the 7" panel is injected here too.

  Usage - include AFTER GUIslice.h (it needs the gslc_ types and the config
  macros), then call gevino_touch_gui_update() at the top of loop():

    #include "gevino_tft_io.h"
    #include "glass/All_76_Pic_65K.h"
    #include "GUIslice.h"
    #include "GUIslice_drv.h"
    #include "gevino_tft_touch.h"
    #include "gevino_tft_touch_gslc.h"

    void setup() {
      gevino_io_setup();
      gevino_touch_setup();                 // BEFORE gslc_Init()
      gslc_Init(&m_gui, &m_drv, ...);
    }

    void loop() {
      gevino_touch_gui_update(&m_gui);      // no-op on a GUIslice-driven 7"
      gslc_Update(&m_gui);
    }
*/

#include "gevino_tft_touch.h"

// A finger is considered lifted when no new frame arrives for this long. The
// controllers re-assert INT on every frame while the finger stays on the glass,
// so "INT high" does not mean "finger lifted" - a timeout does. Without the
// release event GUIslice buttons never fire, because they act on
// GSLC_TOUCH_UP_IN.
#ifndef GEVINO_TOUCH_RELEASE_MS
#define GEVINO_TOUCH_RELEASE_MS  80
#endif

// Poll the touch panel and forward the points to GUIslice.
// Call once per loop(), before gslc_Update().
static inline void gevino_touch_gui_update(gslc_tsGui* pGui)
{
  static bool     bPinArmed = false;
  static bool     bDown     = false;
  static int16_t  nLastX    = 0;
  static int16_t  nLastY    = 0;
  static uint32_t nLastMs   = 0;

  if (!gevino_touch_found()) { return; }

#if defined(DRV_TOUCH_ADA_FT5206)
  // GUIslice drives the 7" panel itself - nothing to do here.
  if (gevino_touch_is7()) { return; }
#endif

  // gslc_Init() constructs GUIslice's own FT5206 driver on the shared INT pin
  // (D38), which reconfigures it. This runs after gslc_Init(), so re-apply the
  // pin setup once for the panel we actually detected.
  if (!bPinArmed) {
    bPinArmed = true;
    gevino_touch_pin_setup();
  }

  int16_t nX, nY;
  if (gevino_touch_read(&nX, &nY)) {
    nLastX  = nX;
    nLastY  = nY;
    nLastMs = millis();
    bDown   = true;
    gslc_TrackTouch(pGui, NULL, nX, nY, 1);
  }

  if (bDown && ((millis() - nLastMs) > GEVINO_TOUCH_RELEASE_MS)) {
    bDown = false;
    gslc_TrackTouch(pGui, NULL, nLastX, nLastY, 0);
  }
}

#endif // _GEVINO_TFT_TOUCH_GSLC_H_
