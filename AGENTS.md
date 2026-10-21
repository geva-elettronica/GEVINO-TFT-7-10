# AGENTS.md — Programming guide for the GEVINO TFT (7" / 10")

> This file is written for **LLMs and AI coding agents** (Claude Code, Cursor, Copilot, etc.) that
> write firmware for the **GEVINO TFT** HMI PLC. It is also useful to human developers. If you are
> generating Arduino/C++ code for this board, follow these rules.

## What the board is

GEVINO TFT is an **Arduino Zero compatible HMI PLC** with a **7" or 10" capacitive-touch TFT**
(1024×600, Raio **RA8876** controller), based on the Microchip **SAMD21** (32-bit ARM Cortex-M0+,
48 MHz, 256 KB flash, 32 KB RAM). It has 12 opto-isolated inputs (4 high-speed/interrupt), 8
opto-isolated outputs (PNP/NPN/AC, 1 A 30 V), 4 analog inputs + 1 analog output, an RS485 bus, a
front USB-stick reader (CH376), a rear micro-SD and optional Ethernet/Wi-Fi/Bluetooth/GSM/LoRa
modules. Program it with the Arduino IDE / Arduino SAMD core (board **Arduino Zero, Native USB
Port**); it also works with Cursor and OpenPLC.

**7" vs 10":** the two models are the **same board** — only the glass changes. The touch controller
differs: **7" = FT5206** (I²C 0x38), **10" = GT9271** (I²C 0x5D). The library handles both and
auto-detects which one is fitted: call `gevino_touch_setup()` once and use the same touch code on
either model (see [Touch](#touch-7-ft5206--10-gt9271)).

The complete pin map is in [README.md](README.md) and in [`src/gevino_tft_io.h`](src/gevino_tft_io.h).

## Golden rules

1. **Write non-blocking, state-based code.** Prefer `switch`/`case` state machines. Avoid `delay()`
   in `loop()`; use `millis()` or the bundled `Timer`.
2. **Respect the include order:**
   ```cpp
   #include "gevino_tft_io.h"    // board I/O, ADC, Timer
   #include "All_76_Pic_65K.h"   // image data (uses the `code`/PROGMEM macro)
   #include "gevino_tft_display.h"
   #include "gevino_tft_touch.h" // touch, if the sketch uses it (7"/10", auto-detected)
   ```
3. **Include `gevino_tft_io.h` in exactly ONE translation unit** (your main `.ino`): it instantiates
   the `Serial2`/`Serial3` SERCOM UARTs, their ISRs and the global I/O registers (`OutReg`/`InReg`).
4. **Do not initialise the I/O yourself.** `gevino_io_setup()` configures every pin, drive strength,
   chip-selects, the shift-register I/O, the touch reset and the ADC/DAC resolution. Call it once in
   `setup()`, **after** `Serial2.begin()` / `Serial3.begin()`.
5. **Use the macros** (`In0`…`In11`, `setOut0`…`setOut7`, `resOut0`…`resOut7`, `setLed`,
   `set_Rs485_TxEn`). Never hard-code raw pin numbers.

## Shift-register I/O — call updateIO()

8 outputs (`Out1`…`Out7`) and 8 inputs (`In4`…`In11`) live in a 74HC595/74HC165 chain, mirrored in
the `OutReg` / `InReg` byte registers. The `setOutN`/`resOutN` macros only change `OutReg` bits and
`In4`…`In11` only read `InReg` bits — you must call **`updateIO()`** to actually push outputs and
sample inputs over SPI. `In0`…`In3` and `Out0` are direct GPIO and do not need `updateIO()`.

```cpp
void loop() {
  updateIO();                       // sample inputs / push outputs
  if (In0 || In4) setOut1; else resOut1;
  updateIO();
}
```

## Minimal sketch skeleton

```cpp
#include "gevino_tft_io.h"
#include "All_76_Pic_65K.h"
#include "gevino_tft_display.h"

void setup() {
  SerialUSB.begin(115200);   // rear USB-C debug console
  Serial1.begin(115200);     // RS485
  gevino_io_setup();
  tft.init();
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.println("GEVINO TFT");
}

void loop() {
  updateIO();
  if (In0) setOut1; else resOut1;
  updateIO();
}
```

## Display (RA8876)

The display is driven by the `tft` object (class `RA8876`, `gevinoTFT_RA8876.h`) over SPI
(`TFT_CS` = 42): `tft.init()`, `tft.clearScreen(color)`, `tft.setTextColor(color)`,
`tft.print()/println()`, geometry and BTE/DMA functions. Full-screen 1024×600 images can be loaded
from the display flash via DMA in ~80 ms (see `DMA_Pics_from_flash`). Fonts and images can be copied
from SD into the display flash (`CopySDonFlash()` in `gevino_tft_display.h`, files `font.bin` /
`All_76_Pic_65K.bin`).

## Touch (7" FT5206 / 10" GT9271)

**Do not write touch code by hand: the library does it.** `gevino_tft_touch.h` drives both
controllers over `Wire` (no external touch library needed), probes both I²C addresses at startup —
7": FT5206 at `0x38`, 10": GT9271 at `0x5D`, INT on D38 / `TouchInt` — and exposes one API that
works with whichever panel is fitted. Coordinates come back **already in screen space**
(0…1023 / 0…599: both panels are mounted rotated 180°, so the library flips and clamps the axes).

```cpp
#include "gevino_tft_io.h"
#include "All_76_Pic_65K.h"
#include "gevino_tft_display.h"
#include "gevino_tft_touch.h"

void setup() {
  gevino_io_setup();       // board bring-up (also pulses the touch reset)
  tft.init();
  gevino_touch_setup();    // probe: returns 7, 10 or 0 (call AFTER gevino_io_setup)
  SerialUSB.println(gevino_touch_name());     // "7\"" / "10\"" / "none"
}

void loop() {
  int16_t x, y;
  if (gevino_touch_read(&x, &y)) tft.fillCircle(x, y, 5, RGB565(0, 255, 0));
}
```

| Function | Description |
|----------|-------------|
| `gevino_touch_setup()` | Probes `0x38` / `0x5D`, configures the INT line, returns `GEVINO_TOUCH_7`, `GEVINO_TOUCH_10` or `GEVINO_TOUCH_NONE` (7 / 10 / 0). |
| `gevino_touch_read(&x, &y)` | `true` + first contact point in screen coordinates when a new frame is pending. Non-blocking. |
| `gevino_touch_points(pts, nMax)` | Multi-touch: fills an array of `GevinoTouchPoint` (`x`, `y`, `id`), returns how many. Up to 5 on the 7", 10 on the 10" (`GEVINO_TOUCH_MAX_POINTS`). |
| `gevino_touch_down()` | `true` while a finger is on the glass (reads the INT line only, no I²C traffic). |
| `gevino_touch_panel()` / `gevino_touch_is7()` / `gevino_touch_is10()` / `gevino_touch_found()` | Which panel was detected. |
| `gevino_touch_name()` | `"7\""`, `"10\""` or `"none"` — ready to print. |
| `gevino_touch_pin_setup()` | Re-applies the INT pin configuration, for when another library reconfigures D38. |

The same sketch therefore runs unchanged on a 7" and a 10" board (see `GEVINO_TFT_Test`,
`Multi_Touch`, `GEVINO_TFT_1.0_Simple`). For a GUIslice GUI see the section below.

## Front USB memory stick (CH376)

The front USB port is a **CH376** USB-host (chip-select `CH376_CS` = 45, interrupt `CH376_INT` = 27)
for reading/writing FAT USB sticks. See `FlashUSB_Stick`.

## Analog inputs and DAC output (non-blocking)

`gevino_io_setup()` sets `analogReadResolution(12)` and `analogWriteResolution(10)`. For the inputs,
use the non-blocking polling (avoid the blocking `analogRead()`):

```cpp
analogPrescaler(ADC_PRESCALER_DIV32);
analogReadExtended(16);
analogGain(ADC_GAIN_1);
analogReference2(ADC_REF_INT1V);
analogCalibrate();
analogEnable();
// then in loop(): analogPolling();  -> analogResult[0..4]
```

The 0–10 V / 4–20 mA analog **output** uses the SAMD21 DAC: `analogWrite(DAC0, value)` (10-bit). Note
the analog output shares one of the analog input channels.

## Serial ports

| Port | Use |
|------|-----|
| `SerialUSB` | rear USB-C — **debug console**. |
| `Serial1`   | **RS485** half-duplex (SERCOM0). |
| `Serial2`   | extra SERCOM UART (SERCOM1, RX 11 / TX 10) — modems / expansion. |
| `Serial3`   | extra SERCOM UART (SERCOM2, RX 3 / TX 4) — SIM800L GSM / expansion. |
| `Serial`    | Wi-Fi (ESP) or Bluetooth module. |

## RS485 (half duplex)

Enable the transmitter with `set_Rs485_TxEn`, send on `Serial1`, then disable it after the last
character has left the UART (the SERCOM0 TXC flag):

```cpp
if (SERCOM0->USART.INTFLAG.bit.TXC) {
  res_Rs485_TxEn;
  SERCOM0->USART.INTFLAG.bit.TXC = 1;
}
```

### Modbus RTU / TCP
Modbus RTU: **ArduinoModbus** + the **GEVA-modified `ArduinoRS485`** (warn the user it must be the
GEVA version), or the `SimpleModbusMaster` used in `Key_ModBus_Inquiry`. Modbus TCP: **ArduinoModbus**
over Ethernet.

## Ethernet / SD / flash

- **Ethernet (W5500):** standard `Ethernet` library, `Ethernet.init(ETH_CS)` (`ETH_CS` = 5).
- **SD card:** **SdFat** (chip-select `SD_CS` = 12). Used for programming, logging, config, images, voice.
- **EEPROM emulation:** **FlashStorage** by cmaglie.

## Timer class

`gevino_tft_io.h` includes a small non-blocking `Timer` based on `millis()`:

- `reset()` restarts the count; while it keeps being reset it never fires.
- `stato()` returns `true` once the configured time has elapsed since `reset()`.
- `fronte()` returns `true` only once (rising edge) when the time elapses.
- `tempo(ms)` sets the period.

## Firmware update and debug

- Update the program **from the SD card** with the dedicated bootloader.
- **Debug** over Ethernet via **Telnet**; **programming** over Ethernet via **FTP** (SD bootloader).

---

## Conventions for building an HTML web interface on the board

When the firmware serves a web UI (e.g. via the W5500 Ethernet module), follow these conventions —
they are tuned for a resource-constrained SAMD21:

### File structure (modular)
1. Separate the content by concern:
   - HTML in `html_content.h`
   - CSS in `css_content.h`
   - JavaScript in `js_content.h`
   - API / JSON in `json_handler.h`
   - main request handling in `html_server.h`
2. Keep sessions/authentication in a dedicated `session_manager.h`, and translations in a dedicated
   `translation.h`.

### Authentication
3. Prefer **GET** for login on Arduino (avoid POST). Use a `pwd` URL parameter: `/login?pwd=xxx`.
4. Read the password from the configuration **immediately before comparing**; never reuse a
   hard-coded password in the code.

### HTML and templates
5. Use complete HTML templates with `R"rawliteral(...)"` and substitutable `{{placeholder}}` tokens.
   Avoid building HTML with many `client.println()` calls.
6. Use a clear HTTP router with separate handling per path; handle `/login` with and without
   parameters separately.

### Multilingual
7. Implement translations as an array of structs. Translate every user-visible string. Auto-detect
   the browser language.

### Debug and security
8. Add `SerialUSB.print()` logging at strategic points. Log both failed and successful auth
   attempts. Log extracted password values **only in debug builds**.
9. Implement timeouts: **sessions 30 minutes**, **HTTP requests 15 seconds max**.

### Resource-constrained compatibility
10. Minimise JavaScript — prefer pure HTML/CSS. Avoid external CSS/JS frameworks that inflate the
    firmware size. Prefer direct links over JavaScript event listeners where possible.

---

## Building a touchscreen GUI with GUIslice (glass theme)

GUIslice (ImpulseAdventure) runs on the GEVINO TFT through a GEVINO-specific display driver
(`DRV_DISP_ADAGFX_RA8876_GV`, which wraps the `gevinoTFT_RA8876` class) plus the FT5206 touch
driver. It is the fastest way to build a full touchscreen HMI: pages, buttons, text, sliders,
gauges, keypads, etc. Reproduce the reference look (welcome splash → blue "glass" theme with
image buttons, white text, a DAC slider with a glass thumb, and a multi-tab font browser) by
following this section verbatim. A complete worked example lives in
[`GUIslice/examples/arduino/GEVINO_TFT/`](../GUIslice/examples/arduino/GEVINO_TFT/).

### One-time setup
- **Libraries:** **GUIslice** (this GEVA fork — it must contain the `DRV_DISP_ADAGFX_RA8876_GV`
  driver and export `RA8876* gslc_GevinoDisp(void)`) and **FT5206** (sumotoy — required by
  GUIslice's own 7" touch driver, not by the GEVINO library). The GEVINO TFT
  library must contain, in `src/`: the orientation `font.h` + `fontExt.h` (copy from
  `src/Horizzontal/` for the 1024×600 landscape set) and `src/glass/All_76_Pic_65K.h`.
- **Config:** in `GUIslice/src/GUIslice_config.h` enable
  `#include "../configs/ard-shld-gevino_tft.h"` (selects RA8876_GV + FT5206; `TFT_CS`=42,
  touch `INT`=38, `SD_CS`=12).
- **Flash:** program `font.bin` (fonts) and the glass `All_76_Pic_65K.bin` (images) into the
  display flash — put them on the SD card and call `CopySDonFlash()` once.
- **Board:** Arduino Zero (Native USB Port).

### Non-negotiable rules
1. **Include order.** The images header must come right after the board header so it supplies
   the `INFO` typedef and `gevinoTFT_RA8876.h` skips its own (`gevino_tft_io.h` defines
   `pic_def` for exactly this reason):
   ```cpp
   #include "gevino_tft_io.h"          // board I/O (also defines pic_def, code)
   #include "glass/All_76_Pic_65K.h"   // flash images: BINARY_INFO + PICTURE_NAME (+ INFO typedef)
   #include "GUIslice.h"
   #include "GUIslice_drv.h"
   #include "elem/XSlider.h"           // only if you use sliders
   #include "fontExt.h"                // GEVA font types + extern declarations
   #include "Horizzontal/font.h"       // GEVA font definitions (orientation-specific)
   #include "gevino_tft_touch.h"       // 7"/10" touch (auto-detected)
   #include "gevino_tft_touch_gslc.h"  // GUIslice bridge (needs the gslc_ types: keep it last)
   extern "C" RA8876* gslc_GevinoDisp(void);   // driver accessor: the live display instance
   ```
   If a project uses **no** flash images, drop the images include and instead `#undef pic_def`
   immediately before `fontExt.h`, so `gevinoTFT_RA8876.h` provides `INFO` itself.
2. **Call `gevino_io_setup()` in `setup()` BEFORE `gslc_Init()`** (it brings up SPI, raises the
   shared chip-selects SD/ETH/CH376 and resets the FT5206). `gslc_Init()` then runs
   `RA8876::init()` (which also copies the fonts to canvas RAM) and `FT5206::begin()`.
3. **Touch:** call `gevino_touch_setup()` right before `gslc_Init()`, and
   `gevino_touch_gui_update(&m_gui)` at the top of `loop()`, before `gslc_Update()`:
   ```cpp
   void setup() { gevino_io_setup(); gevino_touch_setup(); gslc_Init(...); ... }
   void loop()  { gevino_touch_gui_update(&m_gui); gslc_Update(&m_gui); ... }
   ```
   On a **7"** board that call is a no-op — GUIslice polls the FT5206 itself through
   `DRV_TOUCH_ADA_FT5206`. On a **10"** board GUIslice has no GT9271 driver, so the bridge reads
   the panel and injects the points with `gslc_TrackTouch()`, synthesising the release event
   after `GEVINO_TOUCH_RELEASE_MS` (80 ms) of silence — without it buttons never fire, since
   GUIslice acts on `GSLC_TOUCH_UP_IN`. The same sketch runs on both panels.
4. **Route GUIslice debug to `SerialUSB`** via `gslc_InitDebug()` — `Serial` is the Wi-Fi/BT port.
5. The panel is native **1024×600 landscape**, no rotation; color is standard **RGB565**.

### Fonts
`gslc_FontSet(&gui, fontId, GSLC_FONTREF_PTR, &notoSans_32pt, 1)` selects a GEVA flash font.
- GEVA glyphs blit at their **native size** — the size/scale argument is ignored (pass `1`);
  to change size, point at a bigger flash font.
- Families: `notoSans` / `notoSansBold` / `notoSansItal`, `notoSansMono` / `notoSansMonoBold`,
  `notoSerif` / `notoSerifBold`; sizes `8,12,20,32,48,64,80,96,112` pt
  (e.g. `&notoSansBold_48pt`).
- Pass `NULL` instead for the RA8876 internal ROM font (there the scale arg is a 1..4 magnifier).
- You can re-point a font slot at run time (`gslc_FontSet` rewrites it cheaply); after doing so
  call `gslc_InvalidateRgnScreen(&gui); gslc_PageRedrawSet(&gui, true);` to repaint cleanly.

### Glass theme
```cpp
#define GEVA_BLUE_565  RGB565(0x23, 0x90, 0xEC)          // for native RA8876 calls
#define GEVA_BLUE_COL  ((gslc_tsColor){ 0x23, 0x90, 0xEC }) // for GUIslice calls
...
gslc_SetBkgndColor(&gui, GEVA_BLUE_COL);                 // background 0x2390ec
```
**White text without black boxes.** GUIslice text elements default to a *black* fill. Set the
fill to the background blue so it looks transparent AND clears the previous value on update
(essential for any text you change at run time — otherwise old glyphs pile up):
```cpp
gslc_ElemSetTxtCol(&gui, pRef, GSLC_COL_WHITE);
gslc_ElemSetCol(&gui, pRef, GEVA_BLUE_COL, GEVA_BLUE_COL, GEVA_BLUE_COL);
```
(For purely static text on a page that always full-redraws, `gslc_ElemSetFillEn(&gui,pRef,false)`
also works.)

**Glass image buttons.** The flash holds glass button graphics in pressed+released pairs:
rectangles `251/146/105` wide × `70/55/40` tall, and rounds `31/48/60/70` (enum names like
`glass_rect_251_70_released` / `_pressed`). A DMA blit is **unscaled**, so the button rect MUST
equal the image size. Create a normal GUIslice button, then override its drawing with a callback
that blits the glass image (pressed when "glowing") and draws the white label centred on top;
touch still goes through the button's own touch callback:
```cpp
static void DrawGlassBtn(void* pvGui, void* pvElemRef,
                         uint16_t nImgRel, uint16_t nImgPrs,
                         const FONT_INFO* pFont, int16_t w, int16_t h) {
  gslc_tsGui*     pGui  = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pRef  = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pRef);
  RA8876*         tft   = gslc_GevinoDisp();
  int16_t x = pElem->rElem.x, y = pElem->rElem.y;
  tft->pic_b(&BINARY_INFO[gslc_ElemGetGlow(pGui, pRef) ? nImgPrs : nImgRel], x, y);
  const char* s = pElem->pStrBuf;
  if (s && s[0]) {
    int16_t bx, by; uint16_t bw, bh;
    tft->setFont(pFont); tft->setTextColor(WHITE);          // WHITE = transparent bg (chroma key)
    tft->getTextBounds(s, 0, 0, &bx, &by, &bw, &bh);
    tft->setCursor(x + (w - (int16_t)bw) / 2, y + (h - (int16_t)bh) / 2);
    tft->print(s);
  }
  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE);         // a draw callback MUST clear the flag
}
bool CbDrawBtnBig(void* g, void* e, gslc_teRedrawType r) { (void)r;
  DrawGlassBtn(g, e, glass_rect_251_70_released, glass_rect_251_70_pressed, &notoSans_20pt, 251, 70);
  return true; }
...
// label "LED ON" is static -> bufMax 0; for a label you change at run time pass a writable buffer
pRef = gslc_ElemCreateBtnTxt(&gui, E_BTN, E_PG, (gslc_tsRect){x, y, 251, 70}, (char*)"LED ON", 0,
                             E_FONT_ANY, &CbBtn);
gslc_ElemSetDrawFunc(&gui, pRef, &CbDrawBtnBig);
```
`pic_b()`/DMA is self-contained (it doesn't change the canvas / active window / colour depth),
so it interleaves safely with GUIslice's own drawing.

**Slider with a glass thumb.** Override the XSlider drawing only; the XSlider keeps the
touch/drag and the position callback. Match the XSlider thumb size to the image (31):
```cpp
bool CbDrawSlider(void* pvGui, void* pvElemRef, gslc_teRedrawType r) { (void)r;
  gslc_tsGui* pGui = (gslc_tsGui*)pvGui; gslc_tsElemRef* pRef = (gslc_tsElemRef*)pvElemRef;
  gslc_tsElem* pElem = gslc_GetElemFromRef(pGui, pRef); RA8876* tft = gslc_GevinoDisp();
  int16_t x=pElem->rElem.x, y=pElem->rElem.y, w=pElem->rElem.w, h=pElem->rElem.h, T=31;
  int16_t pos = gslc_ElemXSliderGetPos(pGui, pRef);                       // 0..100
  tft->fillRect(x, y, x+w-1, y+h-1, GEVA_BLUE_565);                       // clear (erase old thumb)
  tft->fillRect(x+T/2, y+h/2-2, x+w-1-T/2, y+h/2+2, WHITE);               // track
  tft->pic_b(&BINARY_INFO[glass_round_31_released],
             x + (int16_t)((int32_t)pos * (w-T) / 100), y + (h-T)/2);     // thumb
  gslc_ElemSetRedraw(pGui, pRef, GSLC_REDRAW_NONE); return true; }
...
pRef = gslc_ElemXSliderCreate(&gui, E_SLIDER, E_PG, &sSlider, (gslc_tsRect){x,y,640,50},
                              0, 100, 0, 31, false);
gslc_ElemXSliderSetPosFunc(&gui, pRef, &CbSliderPos);     // CbSliderPos drives analogWrite(DAC0, pos*1023/100)
gslc_ElemSetDrawFunc(&gui, pRef, &CbDrawSlider);
```

**Welcome splash (N seconds).** Make it a page and switch away with a `Timer` in `loop()` — do
not block with `delay()`:
```cpp
Timer tWelcome(5000); bool bWelcome = true;
// in setup(), after building the pages:
gslc_SetPageCur(&gui, E_PG_WELCOME); tWelcome.reset();
// in loop():
gslc_Update(&gui);
if (bWelcome && tWelcome.stato()) { bWelcome = false; gslc_SetPageCur(&gui, E_PG_MAIN); }
```

### Wiring the GUI to the board
Inside touch / position callbacks, use the board macros directly: `setOut0`/`resOut0`,
`setLed`/`resLed`, read `In0`, drive the analog output with `analogWrite(DAC0, value)` (0..1023).
Call `updateIO()` once per `loop()` to refresh the shift-register I/O, and use the bundled
`Timer` class for periodic UI updates. This is how the GUI becomes a real PLC HMI: buttons and
sliders read inputs and drive opto outputs / the DAC, all non-blocking.

### Sizing reminders for `gslc_Init`
`MAX_PAGE`, the per-page element arrays (`gslc_tsElem` / `gslc_tsElemRef`) and `MAX_FONT` must be
big enough for everything you create. A custom-drawn slider needs `gslc_tsXSlider` storage.
Keep totals modest — the SAMD21 has 32 KB RAM.
