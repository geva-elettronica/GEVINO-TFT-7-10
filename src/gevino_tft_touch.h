#ifndef _GEVINO_TFT_TOUCH_H_
#define _GEVINO_TFT_TOUCH_H_

/*
  GEVINO TFT - capacitive touch support (7" and 10")
  GEVA Elettronica - https://www.gevaelettronica.it

  The 7" and 10" GEVINO TFT are the SAME board: only the glass changes, and
  with it the capacitive touch controller:
    -  7" : FT5206, I2C 0x38, up to  5 simultaneous touches
    - 10" : GT9271, I2C 0x5D, up to 10 simultaneous touches
  Both share the same interrupt line, D38 (`TouchInt`), pulled low by the
  controller while it has a touch frame ready to be read.

  This module hides that difference: it probes both I2C addresses at start-up,
  remembers which panel answered, and exposes ONE set of functions that work
  with either controller. Coordinates are already rotated to screen space
  (both panels are mounted 180 deg rotated with respect to the display memory)
  and clamped to 1024 x 600, so they can be used directly with the `tft`
  drawing primitives.

  No external touch library is needed: both controllers are driven over `Wire`.

  Usage (respect this include order):
    #include "gevino_tft_io.h"
    #include "All_76_Pic_65K.h"
    #include "gevino_tft_display.h"
    #include "gevino_tft_touch.h"

    void setup() {
      gevino_io_setup();        // board bring-up (also resets the touch panel)
      tft.init();
      gevino_touch_setup();     // probe 0x38 / 0x5D, returns 7, 10 or 0
    }

    void loop() {
      int16_t x, y;
      if (gevino_touch_read(&x, &y)) tft.fillCircle(x, y, 5, RGB565(0,255,0));
    }

  For multi-touch use gevino_touch_points(); for a GUIslice GUI include
  "gevino_tft_touch_gslc.h" as well (after GUIslice.h) and call
  gevino_touch_gui_update(&m_gui) at the top of loop().

  IMPORTANT:
    - Call gevino_touch_setup() AFTER gevino_io_setup(): the board bring-up
      pulses the touch reset line and leaves D38 as an input pull-up.
    - This module calls Wire.begin(); it never changes the I2C clock, so a
      sketch is free to set its own with Wire.setClock() afterwards.

  Changelog
    11/08/2026   Touch handling and 7"/10" detection moved into the library.
*/

#include <Arduino.h>
#include <Wire.h>

// The interrupt line (D38) is shared by both controllers. gevino_tft_io.h
// defines TouchInt too, with the same value; this definition is what the
// library .cpp uses, since it does not include the board header.
#ifndef TouchInt
#define TouchInt 38
#endif

// I2C addresses of the two controllers
#define GEVINO_TOUCH_FT5206_ADDR  0x38   // 7"  panel
#define GEVINO_TOUCH_GT9271_ADDR  0x5D   // 10" panel

// Native panel size (both models are 1024x600)
#ifndef GEVINO_TOUCH_WIDTH
#define GEVINO_TOUCH_WIDTH   1024
#endif
#ifndef GEVINO_TOUCH_HEIGHT
#define GEVINO_TOUCH_HEIGHT  600
#endif

// Largest number of simultaneous touches reported by either controller
#define GEVINO_TOUCH_MAX_POINTS  10

// Which glass is fitted. The values are the panel size in inches, so they can
// be printed as-is.
enum {
  GEVINO_TOUCH_NONE = 0,   // nothing answered on the I2C bus
  GEVINO_TOUCH_7    = 7,   // 7"  panel, FT5206
  GEVINO_TOUCH_10   = 10   // 10" panel, GT9271
};

// One contact point, already in screen coordinates (0..1023, 0..599).
struct GevinoTouchPoint {
  int16_t x;
  int16_t y;
  uint8_t id;    // controller-assigned finger id (tracks a finger while it moves)
};

// -----------------------------------------------------------------------------
// Detection / setup
// -----------------------------------------------------------------------------

// Probe both controllers and configure the interrupt line for the one that
// answered. Call once in setup(), after gevino_io_setup().
// Returns GEVINO_TOUCH_7, GEVINO_TOUCH_10 or GEVINO_TOUCH_NONE.
uint8_t gevino_touch_setup(void);

// Re-apply the interrupt-line pin configuration for the detected panel, without
// probing again. Needed only when another library reconfigures D38 after
// gevino_touch_setup() (e.g. GUIslice constructing its own FT5206 driver).
void gevino_touch_pin_setup(void);

// Panel detected by the last gevino_touch_setup(): 7, 10 or 0.
uint8_t gevino_touch_panel(void);

bool gevino_touch_found(void);   // a panel answered
bool gevino_touch_is7(void);     // 7"  panel (FT5206)
bool gevino_touch_is10(void);    // 10" panel (GT9271)

// Human-readable panel name: "7\"" (FT5206), "10\"" (GT9271) or "none".
const char* gevino_touch_name(void);

// -----------------------------------------------------------------------------
// Reading
// -----------------------------------------------------------------------------

// True while at least one finger is on the glass (reads the shared INT line, no
// I2C traffic). Useful to skip the I2C read when nothing is happening.
bool gevino_touch_down(void);

// Read the first contact point. Returns true and fills *pX / *pY (screen
// coordinates) when a fresh touch frame was available, false otherwise.
bool gevino_touch_read(int16_t* pX, int16_t* pY);

// Read up to nMax contact points into pLoc (5 max on the 7", 10 on the 10").
// Returns the number of points actually written.
uint8_t gevino_touch_points(GevinoTouchPoint* pLoc, uint8_t nMax);

#endif // _GEVINO_TFT_TOUCH_H_
