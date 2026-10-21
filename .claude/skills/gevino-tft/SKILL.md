---
name: gevino-tft
description: Use when writing Arduino/C++ firmware for the GEVA GEVINO TFT HMI PLC (7" or 10" capacitive-touch, 1024x600, Raio RA8876, SAMD21, Arduino Zero compatible). Covers the single-header board I/O, shift-register I/O (updateIO), the RA8876 display, 7"/10" touch auto-detection (FT5206/GT9271), front USB stick (CH376), DMA images, non-blocking ADC + DAC output, RS485 + Modbus, Ethernet (W5500), SD (SdFat), the Timer class, building a touchscreen GUI with GUIslice (glass theme), and HTML web-UI conventions.
---

# GEVINO TFT firmware

You are writing firmware for the **GEVINO TFT** — an Arduino Zero compatible HMI PLC with a 7" or 10"
capacitive-touch TFT (1024×600, Raio RA8876 controller) based on the Microchip SAMD21 (ARM
Cortex-M0+, 48 MHz, 256 KB flash, 32 KB RAM), made by GEVA Elettronica. Build in the Arduino IDE /
arduino-cli with the **Arduino SAMD core**, board **Arduino Zero (Native USB Port)**; it also works
with Cursor and OpenPLC.

**7" vs 10":** same board, only the glass changes. Touch controller: 7" = FT5206 (I²C 0x38), 10" =
GT9271 (I²C 0x5D). The library drives both and auto-detects the panel — never write touch code by
hand, call `gevino_touch_setup()` once and the same sketch runs on either model.

## Read these first
- [`AGENTS.md`](../../../AGENTS.md) — the full programming guide (this skill is its short form).
- [`src/gevino_tft_io.h`](../../../src/gevino_tft_io.h) — board I/O macros, ADC helpers, Timer.
- [`src/gevino_tft_touch.h`](../../../src/gevino_tft_touch.h) — the 7"/10" touch API.
- [`src/gevinoTFT_RA8876.h`](../../../src/gevinoTFT_RA8876.h) — the RA8876 display driver (`tft` object).

## Non-negotiable rules
1. **Respect the include order:**
   ```cpp
   #include "gevino_tft_io.h"
   #include "All_76_Pic_65K.h"
   #include "gevino_tft_display.h"
   ```
2. **Include `gevino_tft_io.h` in exactly ONE translation unit** (the main `.ino`): it instantiates
   the Serial2/Serial3 SERCOM UARTs, their ISRs and the global I/O registers (`OutReg`/`InReg`).
3. **Call `gevino_io_setup()` once in `setup()`**, after `Serial2.begin()`/`Serial3.begin()`; do not
   initialise pins yourself.
4. **Use the macros**: inputs `In0`…`In11`, outputs `setOut0`…`setOut7` / `resOut0`…`resOut7`,
   `setLed`/`resLed`, `set_Rs485_TxEn`/`res_Rs485_TxEn`.
5. **Call `updateIO()`** to push outputs (`Out1`…`Out7`) and sample inputs (`In4`…`In11`): these go
   through a 74HC595/74HC165 shift-register chain mirrored in `OutReg`/`InReg`. `In0`…`In3` and
   `Out0` are direct GPIO. Write non-blocking, state-based code; avoid `delay()` in `loop()`.

## Skeleton

```cpp
#include "gevino_tft_io.h"
#include "All_76_Pic_65K.h"
#include "gevino_tft_display.h"

void setup() {
  SerialUSB.begin(115200);
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

## Subsystems (see AGENTS.md for full code)
- **Display:** `tft` (class `RA8876`, `TFT_CS` = 42): `tft.init/clearScreen/setTextColor/print`,
  DMA full-screen images (~80 ms) from display flash; copy fonts/images from SD with `CopySDonFlash()`.
- **Touch:** `gevino_tft_touch.h` drives FT5206 (7", 0x38) and GT9271 (10", 0x5D) over `Wire`
  (`TouchInt` = D38, no external library). `gevino_touch_setup()` probes and returns 7/10/0;
  `gevino_touch_read(&x, &y)` gives one point, `gevino_touch_points(pts, n)` multi-touch — both in
  screen coordinates (0…1023 / 0…599, axes already flipped). Also `gevino_touch_name()`,
  `gevino_touch_is7()/is10()`, `gevino_touch_down()`.
- **USB stick:** CH376 host (`CH376_CS` = 45, `CH376_INT` = 27) — see FlashUSB_Stick example.
- **Analog:** non-blocking `analogPolling()` → `analogResult[0..4]`; 10-bit DAC output via
  `analogWrite(DAC0, ...)`. ADC helpers (`analogReadExtended`, `analogGain`, …) bundled in the header.
- **Serial/RS485:** `SerialUSB` debug; `Serial1` RS485 (SERCOM0, use `set_Rs485_TxEn` then
  `res_Rs485_TxEn` after `SERCOM0->USART.INTFLAG.bit.TXC`); `Serial2`/`Serial3` modems; `Serial`
  Wi-Fi/BT. Modbus RTU via ArduinoModbus + GEVA ArduinoRS485 (or SimpleModbusMaster).
- **Ethernet (W5500):** `Ethernet.init(ETH_CS)` (`ETH_CS` = 5). **SD:** SdFat (`SD_CS` = 12).
  **EEPROM emulation:** FlashStorage by cmaglie.
- **Timer:** non-blocking `Timer` (`reset`/`stato`/`fronte`/`tempo`).

## Touchscreen GUI with GUIslice (recommended for HMIs)
To build a touchscreen UI (pages, buttons, text, sliders, gauges, keypads) use **GUIslice** with
the GEVINO driver `DRV_DISP_ADAGFX_RA8876_GV` + FT5206 touch. Enable
`configs/ard-shld-gevino_tft.h` in `GUIslice_config.h`, call `gevino_io_setup()` then
`gevino_touch_setup()` before `gslc_Init()`, and respect the include order (`gevino_tft_io.h` →
`glass/All_76_Pic_65K.h` → GUIslice → `fontExt.h` → orientation `font.h` → `gevino_tft_touch.h` →
`gevino_tft_touch_gslc.h`). Call `gevino_touch_gui_update(&gui)` at the top of `loop()`, before
`gslc_Update()`: it is a no-op on the 7" (GUIslice polls the FT5206 itself) and injects the GT9271
points on the 10". The full recipe — GEVA flash fonts, the **glass
theme** (0x4c9de4 bg, white text, glass image buttons via `gslc_GevinoDisp()` + a draw callback +
`pic_b()`, glass-round slider thumb, welcome splash) — is in the **"Building a touchscreen GUI with
GUIslice (glass theme)" section of AGENTS.md**, with a worked example in
`GUIslice/examples/arduino/GEVINO_TFT/`.

## HTML web UI
If you build a web interface, follow the resource-constrained conventions in **AGENTS.md**
(modular `*_content.h` files, GET-based login with a `pwd` param, `R"rawliteral(...)"` templates
with `{{placeholders}}`, struct-array translations, 30-min session / 15-s request timeouts, minimal
JavaScript).
