# GUIslice on the GEVINO TFT — basic example

First, simple step of the GUIslice port to the **GEVINO TFT** (7" / 10") HMI PLC by
GEVA Elettronica: **display driver + basic GUIslice commands** (box, text, buttons,
touch) wired together with the GEVINO board I/O.

This example uses the **GEVA external fonts** programmed into the display serial flash
(`notoSans_12pt`). Images are still handled differently on the GEVINO TFT and are the
remaining step.

## Theme

A "glass" theme: solid **0x2390ec** blue background, **white** text everywhere, and the
buttons use the **glass images stored in the display flash** (pressed/released states).
On boot a **5-second welcome splash** shows *GEVA elettronica / gevino TFT /
shop.gevaelettronica.it / glass theme*. The main page also has a row of **8 red glass
LEDs** mirroring the 8 outputs (Out0..Out7), so the digital light show is visible on
screen as well as on the physical outputs.

The glass button images are blitted by DMA from the display flash via the RA8876's
`pic_b()`. GUIslice does not wrap that, so the driver now exports
`gslc_GevinoDisp()` (returns the live `RA8876*`); each image button uses a GUIslice
custom draw callback (`gslc_ElemSetDrawFunc`) that blits the glass image and renders the
white label centred on top. Button rects must match the image size (251×70 / 105×55) since
the DMA blit is unscaled.

## What it shows

A 1024×600 page with:
- a title and a framed box,
- a live counter (updated every second by the GEVINO `Timer`),
- the state of opto input **In0**,
- three buttons: **LED ON** / **LED OFF** (front yellow LED) and **OUT0** (toggles the
  opto-isolated output `Out0` and updates its own label),
- a **slider** (custom-drawn: blue background, white track, the smallest glass round
  image as the thumb) with an **ANALOG / DIGITAL** toggle button to its right:
  - *ANALOG*: drives the GEVINO 10-bit analog output (`analogWrite(DAC0, …)`), value shown live;
  - *DIGITAL*: lights one of the 8 outputs (Out0..Out7) — the lit light follows the slider as
    you move it; **hold the thumb still** and the lights **run** (chase); **release** and they
    **oscillate** (−1, −2, +1, +2) before settling on the selected light,
- a **FONTS** button that opens a second page — a font gallery with one **tab per
  family** (Sans, Bold, Italic, Mono, Mono Bold, Serif, Serif Bold) and **all sizes**
  of that family shown in white (8…112pt). Switching tab re-points 9 reusable font
  slots to the selected family (`gslc_FontSet` at run time) instead of allocating a
  separate font per size. The largest sizes sit in the right column; 112pt is taller
  than the panel and is clipped at the bottom.

## Touch (7" and 10")

The sketch runs unchanged on both models. `gevino_touch_setup()` (from the GEVINO TFT library)
probes the I²C bus before `gslc_Init()` and remembers which panel is fitted; in `loop()`,
`gevino_touch_gui_update(&m_gui)` feeds it to GUIslice:

- **7" (FT5206, 0x38)** — GUIslice polls it itself through `DRV_TOUCH_ADA_FT5206`, so the bridge
  call is a no-op.
- **10" (GT9271, 0x5D)** — GUIslice has no driver for it, so the library reads the panel and injects
  the points with `gslc_TrackTouch()`, synthesising the release event after 80 ms of silence (both
  controllers keep re-asserting INT while the finger is down, so "INT high" does not mean "finger
  lifted" — and without a release GUIslice buttons never fire, since they act on `GSLC_TOUCH_UP_IN`).

None of that lives in the sketch: it is all in `gevino_tft_touch.h` / `gevino_tft_touch_gslc.h`.

## Requirements

1. **Board:** *Arduino Zero (Native USB Port)* — Arduino SAMD core.
2. **Libraries** (installed next to GUIslice):
   - **GEVINO TFT** library — provides `gevino_tft_io.h` and the `gevinoTFT_RA8876`
     display driver (the `RA8876 tft`/`m_disp` class). It must also contain the
     orientation-specific **`font.h`** + **`fontExt.h`** in its `src/` folder (copy them
     from `src/Horizzontal/` for the 1024×600 landscape set — same as a normal GEVA
     project), and the matching **`font.bin`** must be programmed into the display flash.
     It must also provide **`src/glass/All_76_Pic_65K.h`** (the glass image table), and the
     matching **`All_76_Pic_65K.bin`** (glass theme) must be programmed into the display
     flash at `Pic_Offset`.
   - **FT5206** — sumotoy capacitive-touch library (`<FT5206.h>`), required by GUIslice's own 7"
     touch driver. The GEVINO library does not need it: it drives both panels over `Wire`.
3. **Config:** in [`GUIslice/src/GUIslice_config.h`](../../../src/GUIslice_config.h) make sure
   this line is enabled (it already is in this repo):
   ```cpp
   #include "../configs/ard-shld-gevino_tft.h"
   ```
   It selects `DRV_DISP_ADAGFX_RA8876_GV` (display) + `DRV_TOUCH_ADA_FT5206` (touch)
   with the GEVINO pinout (`TFT_CS = 42`, touch `INT = 38`, `SD_CS = 12`).

## How the pieces fit

| Layer            | Provided by                                   |
|------------------|-----------------------------------------------|
| GUI logic        | GUIslice (`gslc_*` API)                        |
| Display driver   | `gevinoTFT_RA8876` via `DRV_DISP_ADAGFX_RA8876_GV` |
| Touch 7"         | sumotoy FT5206 via `DRV_TOUCH_ADA_FT5206`      |
| Touch 10"        | `gevino_tft_touch.h` + `gevino_tft_touch_gslc.h` (GEVINO TFT library) |
| Panel detection  | `gevino_touch_setup()` (GEVINO TFT library)    |
| Board I/O        | `gevino_tft_io.h` (`gevino_io_setup()`, `updateIO()`, `In*/Out*`, `Timer`) |

`gevino_io_setup()` performs the board bring-up (SPI + drive strength, raises the shared
chip-selects, resets the touch panel) and must be called **before** `gslc_Init()`, followed by
`gevino_touch_setup()`. `gslc_Init()` then runs `RA8876::init()` and `FT5206::begin()`.

## Notes / limitations (this first step)

- Color format is standard **RGB565** (matches the RA8876 drawing primitives).
- Display rotation is fixed to the native **1024×600** landscape (the driver reports no
  rotation support, so `GSLC_ROTATE` is ignored).
- Fonts: `gslc_FontSet(..., &notoSans_12pt, 1)` selects a GEVA flash font. GEVA glyphs are
  blitted at their native size, so the size argument is ignored — to enlarge text, point
  at a bigger flash font (`&notoSans_20pt`, `&notoSans_32pt`, …). Passing `NULL` instead
  falls back to the RA8876 internal ROM font, where the size argument is a 1..4 scale.
