/*
 * GEVINO TFT
 * Multi Touch Test
 *
 * Draws a coloured dot under every finger on the glass, one colour per
 * contact point.
 *
 * The sketch does not care which panel is fitted: the library probes the
 * I2C bus at start-up and reads whichever controller answered -
 *   -  7" : FT5206, up to  5 simultaneous touches
 *   - 10" : GT9271, up to 10 simultaneous touches
 * gevino_touch_points() returns the contacts already converted to screen
 * coordinates (0..1023, 0..599).
 */

#include "gevino_tft_io.h"  // Respect this sequence
#include "All_76_Pic_65K.h" // Keep this include order
#include "gevino_tft_display.h"
#include "gevino_tft_touch.h"

// One colour per contact point (the 10" reports up to 10 of them)
const uint16_t TOUCH_COLOR[GEVINO_TOUCH_MAX_POINTS] = {
  RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, RED, GREEN, BLUE
};

void setup() {
  SerialUSB.begin(115200);
//  while(!SerialUSB);   // Wait for the USB serial from the PC to open

  gevino_io_setup();
  tft.init();
  tft.selectInternalFont(RA8876_FONT_SIZE_32);

  // Probe both controllers and initialise the one that is there.
  uint8_t panel = gevino_touch_setup();

  SerialUSB.print("Touch panel: ");
  SerialUSB.println(gevino_touch_name());
  tft.print("Touch panel: ");
  tft.println(gevino_touch_name());

  if (!panel) {
    tft.println("No touch controller found");
    SerialUSB.println("No touch controller found");
  } else {
    tft.print(gevino_touch_is10() ? "GT9271 - up to 10 fingers"
                                  : "FT5206 - up to 5 fingers");
  }
}

void loop() {
  GevinoTouchPoint points[GEVINO_TOUCH_MAX_POINTS];

  // Non-blocking: returns 0 when no new touch frame is pending.
  uint8_t count = gevino_touch_points(points, GEVINO_TOUCH_MAX_POINTS);

  for (uint8_t i = 0; i < count; i++) {
    tft.fillCircle(points[i].x, points[i].y, 3, TOUCH_COLOR[i]);

    SerialUSB.print("touch ");   SerialUSB.print(i);
    SerialUSB.print(" (id ");    SerialUSB.print(points[i].id);
    SerialUSB.print(") x=");     SerialUSB.print(points[i].x);
    SerialUSB.print(" y=");      SerialUSB.println(points[i].y);
  }
}
