# GEVINO TFT (7" / 10")

**GEVINO TFT** is an affordable, Arduino-compatible **HMI PLC** with a **7-inch or 10-inch
capacitive-touch TFT** display (1024×600), built around the Microchip **SAMD21** microcontroller
(32-bit ARM Cortex-M0+) and a **Raio RA8876** display controller. It is **Arduino Zero compatible**,
so you program it with the standard Arduino IDE / Arduino SAMD core (it also works with Cursor and
OpenPLC), while getting industrial opto-isolated I/O, analog I/O, an RS485 bus, an SD card and a
front USB-stick reader.

The **7" and 10" models are the same board** — only the **glass changes** (the TFT panel and its
capacitive-touch controller): the 7" panel uses an **FT5206** touch controller, the 10" panel a
**GT9271**. The firmware auto-detects which panel is fitted (see the test example).

Designed and manufactured by **[GEVA Elettronica](https://www.gevaelettronica.it)** (Italy).

> Looking to program the board? Jump to **[Quick start](#quick-start)**.
> Writing firmware with an AI coding assistant? Read **[AGENTS.md](AGENTS.md)** — the LLM/agent
> programming guide for this board.

[![GEVINO TFT demo video](https://img.youtube.com/vi/IzdNAdZBxVE/maxresdefault.jpg)](https://youtu.be/IzdNAdZBxVE)

▶ **[Watch the GEVINO TFT demo on YouTube](https://youtu.be/IzdNAdZBxVE)**

---

## Highlights

- **7" or 10" capacitive-touch TFT**, 1024×600, brightness 400 cd/m² (RA8876 controller).
- **Arduino Zero compatible** — Arduino IDE / Arduino SAMD core (also Cursor, OpenPLC).
- **12 opto-isolated inputs**, 4 of them high-speed with interrupt.
- **8 opto-isolated outputs**, PNP / NPN / AC, 1 A 30 V.
- **4 analog inputs** 0–10 V or 4–20 mA; **1 analog output** 0–10 V or 4–20 mA (uses one analog input).
- **RS485** half-duplex.
- **Front USB** port for reading/writing a USB memory stick.
- **Rear SD card** (programming, logging, configuration, images, voice) and **USB-C** (programming/debug).
- **DMA image loading**: a full 1024×600 color image in ~80 ms.
- Power supply **12 V / 24 V**, removable 3.5 mm terminal blocks.
- Options: DC-DC isolator, 220 V PSU, RTC, RS232, SIM800L GSM (with audio), Wi-Fi, Bluetooth,
  Ethernet, LoRa, a 27 V 43 A DC motor driver with current reading, acrylic panel frame, rear cover.

---

## Technical specifications

### Display

| | |
|---|---|
| Sizes | **7"** and **10"** (same board, different glass) |
| Resolution | 1024 × 600, capacitive multi-touch |
| Controller | Raio **RA8876** (SPI) |
| Touch controller | 7": **FT5206** (I²C 0x38) — 10": **GT9271** (I²C 0x5D) |
| Viewing angle | L/R 70°, top 60°, bottom 70° |
| Brightness | 400 cd/m² |
| DMA | full-screen 1024×600 image load in ~80 ms |

### Inputs / Outputs

- **12 opto-isolated inputs**, 4 of which are high-speed with interrupt.
- **8 opto-isolated outputs**: PNP, NPN and AC, **1 A 30 V**.
- **4 analog inputs**: 0–10 V or 4–20 mA.
- **1 analog output**: 0–10 V or 4–20 mA (uses one analog input).
- **RS485** half-duplex.

### Connectivity and front panel

- **Front USB** port (USB memory stick read/write, CH376 host controller).
- **Rear micro-SD** card (programming, logging, configuration, images, voice).
- **Rear USB-C** port (programming and debugging).
- Removable terminal blocks, 3.5 mm pitch. Power supply **12 V / 24 V**.

### Processor

- 32-bit **ARM Cortex-M0+**, 48 MHz (Microchip SAMD21)
- Flash memory **256 KB**, RAM **32 KB**

### Optional modules

DC-DC isolator 12/24 V → 12 V, internal 220 V → 12 V PSU, RTC, RS232, SIM800L GSM modem with audio
output, Wi-Fi (internal or external antenna), Bluetooth, Ethernet (W5500), LoRa transceiver, 27 V
43 A DC motor driver with current reading, black acrylic panel-mount frame, rear board cover.

### Update and debug

- Update the program **from the SD card** (with the dedicated bootloader).
- **Debug via Ethernet Telnet.**
- **Programming via Ethernet FTP** (requires the SD bootloader).

---

## Pinout / pin mapping

Functions are exposed through named macros in [`src/gevino_tft_io.h`](src/gevino_tft_io.h). The 8
outputs and 8 of the inputs are handled through a **74HC595 (output) / 74HC165 (input) shift-register
chain** driven over SPI (refresh with `updateIO()`); the remaining 4 inputs are direct GPIO. Pin
numbers refer to the Arduino SAMD (Arduino Zero) numbering.

### Inputs (active = closed)

| Macro | Source | | Macro | Source |
|-------|--------|---|-------|--------|
| `In0` | pin 18 (A4), direct | | `In6`  | shift reg `InReg` bit 2 |
| `In1` | pin 19 (A5), direct | | `In7`  | shift reg `InReg` bit 3 |
| `In2` | pin 8, direct | | `In8`  | shift reg `InReg` bit 4 |
| `In3` | pin 9, direct | | `In9`  | shift reg `InReg` bit 5 |
| `In4` | shift reg `InReg` bit 0 | | `In10` | shift reg `InReg` bit 6 |
| `In5` | shift reg `InReg` bit 1 | | `In11` | shift reg `InReg` bit 7 |

The direct inputs `In0`…`In3` are the high-speed / interrupt-capable ones.

### Outputs

| Macro | Source | Set / Reset |
|-------|--------|-------------|
| `Out0` | pin 44 (PA30), direct | `setOut0` / `resOut0` |
| `Out1` … `Out7` | shift reg `OutReg` bits 1–7 | `setOut1`…`setOut7` / `resOut1`…`resOut7` |

Call **`updateIO()`** to push `OutReg` to the 74HC595 and read the 74HC165 into `InReg`.

### Analog

`analogPolling()` fills `analogResult[]` (non-blocking): `analogResult[0]`…`analogResult[4]` are the
analog inputs. The analog output uses the SAMD21 DAC (`analogWrite`).

### System / bus pins

| Macro | Arduino pin | Function |
|-------|-------------|----------|
| `TFT_CS` | 42 | RA8876 display chip-select |
| `SD_CS` | 12 | micro-SD chip-select |
| `ETH_CS` | 5 | W5500 Ethernet chip-select |
| `CH376_CS` | 45 | CH376 USB-host chip-select |
| `CH376_INT` | 27 | CH376 interrupt |
| `TouchInt` | 38 | capacitive-touch interrupt |
| `latchPinO` | 6 | 74HC595 output latch |
| `latchPinI` | 2 | 74HC165 input latch |
| `Rs485_TxEn` | 7 | RS485 transmit enable — `set_Rs485_TxEn` / `res_Rs485_TxEn` |
| `LED_BUILTIN` | — | yellow front LED — `setLed` / `resLed` |

### Serial ports

| Port | Use |
|------|-----|
| `SerialUSB` | rear USB-C — **debug / programming console** |
| `Serial1`   | **RS485** half-duplex bus |
| `Serial2`   | extra SERCOM UART (SERCOM1, RX 11, TX 10) — modems / expansion |
| `Serial3`   | extra SERCOM UART (SERCOM2, RX 3, TX 4) — SIM800L GSM / expansion |
| `Serial`    | Wi-Fi (ESP) or Bluetooth module |

---

## Repository contents

| Path | Description |
|------|-------------|
| [`src/`](src/) | Board I/O header (`gevino_tft_io.h`), the RA8876 display driver (`gevinoTFT_RA8876.*`), the display helper (`gevino_tft_display.h`) and the font tables. |
| [`examples/`](examples/) | Ready-to-flash Arduino sketches (display, touch, DMA images, analog/DAC, USB stick, Modbus, full test). |
| [`docs/`](docs/) | Hardware manual, CE declaration, RA8876 datasheets/API, panel-cut drawings and the font binary. |
| [`AGENTS.md`](AGENTS.md) | Programming guide for LLMs / AI coding agents. |
| [`llms.txt`](llms.txt) | LLM-friendly map of the repository (llmstxt.org convention). |
| [`keywords.txt`](keywords.txt) | Arduino IDE syntax highlighting. |
| [`library.properties`](library.properties) | Arduino library metadata. |

---

## Requirements

- **Arduino IDE** (or arduino-cli) and the **Arduino SAMD core**; board **Arduino Zero (Native USB Port)**.
- The board I/O, ADC helpers and Timer are bundled in `gevino_tft_io.h` (no external dependency).
- Examples use additional libraries depending on what they exercise (e.g. **SdFat**, **Ethernet**,
  **DS3231** RTC, **FRAM_I2C**, ESP Wi-Fi). Each sketch documents what it needs.

## Quick start

Respect the include order (the image data and the display helper rely on macros defined by the I/O
header):

```cpp
#include "gevino_tft_io.h"   // board I/O, ADC, Timer (include in ONE .ino)
#include "All_76_Pic_65K.h"  // image data
#include "gevino_tft_display.h"

void setup() {
  SerialUSB.begin(115200);
  gevino_io_setup();          // configures all the board I/O (call AFTER Serial2/3.begin())
  tft.init();                 // bring up the RA8876 display
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.println("Hello GEVINO TFT");
}

void loop() {
  updateIO();                 // refresh the shift-register I/O
  if (In0) setOut1; else resOut1;
  updateIO();
}
```

> **Important:** include `gevino_tft_io.h` in exactly **one** translation unit (your main `.ino`); it
> instantiates the `Serial2`/`Serial3` SERCOM UARTs and the global I/O registers.

Write **non-blocking, state-based** code (prefer `switch`/`case`, avoid `delay()` in `loop()`).
See [AGENTS.md](AGENTS.md) for the full programming method, display/touch, RS485, USB-stick, SD,
analog and DMA-image recipes.

## API reference (board I/O)

### Digital I/O
- Inputs: `In0` … `In11` (active = closed). Refresh shift-register inputs with `updateIO()`.
- Outputs: `setOut0`…`setOut7`, `resOut0`…`resOut7`; push with `updateIO()`.
- `setLed` / `resLed`, `set_Rs485_TxEn` / `res_Rs485_TxEn`.

### Functions
| Function | Description |
|----------|-------------|
| `gevino_io_setup()` | Configures pins, drive strength, the SERCOM UARTs, chip-selects, shift-register I/O, touch reset and ADC/DAC resolution. Call it in `setup()`. |
| `updateIO()` | Pushes `OutReg` to the 74HC595 outputs and reads the 74HC165 inputs into `InReg`. |
| `analogEnable()` | Starts the first non-blocking ADC conversion. |
| `analogPolling()` | Non-blocking analog read; updates `analogResult[]`. Call it often in `loop()`. |

### SAMD21 ADC helpers
Bundled in the header: `analogReadExtended`, `analogGain`, `analogReference2`, `analogCalibrate`,
`analogPrescaler`, `analogReset`, `analogDifferential`, …

### Display
The RA8876 controller is driven by the `tft` object (class `RA8876` in `gevinoTFT_RA8876.h`):
`tft.init()`, `tft.clearScreen()`, `tft.setTextColor()`, `tft.print()/println()`, DMA image
functions, etc. See the examples.

### Timer
A lightweight non-blocking software timer class (`Timer`) based on `millis()` is included.

## Examples

| Example | What it shows |
|---------|---------------|
| [`GEVINO_TFT_Test`](examples/GEVINO_TFT_Test/GEVINO_TFT_Test.ino) | Full self-test with **7"/10" touch auto-detection**, plus USB flash, CAN, Ethernet, RTC, FRAM, analog I/O, ESP, SIM800L and shift-register I/O. |
| [`Multi_Touch`](examples/Multi_Touch/Multi_Touch.ino) | Multi-touch input. |
| [`DMA_Pics_from_flash`](examples/DMA_Pics_from_flash/DMA_Pics_from_flash.ino) | Fast full-screen image loading via DMA from the display flash. |
| [`Demo`](examples/Demo/Demo.ino) | Graphical HMI demo. |
| [`ADC`](examples/ADC/ADC.ino) / [`DAC`](examples/DAC/DAC.ino) | Non-blocking analog input / analog output. |
| [`FlashUSB_Stick`](examples/FlashUSB_Stick/FlashUSB_Stick.ino) | Reading/writing a front USB memory stick (CH376). |
| [`FontChipFlash`](examples/FontChipFlash/FontChipFlash.ino) | Loading fonts into the display flash. |
| [`Interrupt`](examples/Interrupt/Interrupt.ino) | High-speed interrupt inputs. |
| [`Key_ModBus_Inquiry`](examples/Key_ModBus_Inquiry/Key_ModBus_Inquiry.ino) | Modbus master over RS485. |
| [`GEVINO_TFT_Horizzontal`](examples/GEVINO_TFT_Horizzontal/) / [`GEVINO_TFT_Vertical`](examples/GEVINO_TFT_Vertical/) | Landscape / portrait orientation. |

## Documentation

The hardware manual, the CE declaration, the RA8876 datasheet and API note, the panel-cut drawings
and the font binary are in [`docs/`](docs/).

## Buy / contact

GEVINO PLCs are designed and produced by **GEVA Elettronica**.

- Website: https://www.gevaelettronica.it
- Email: info@gevaelettronica.it

## License

MIT — see [LICENSE](LICENSE). The bundled SAMD21 ADC helpers are derived from Blake Felt's
`ATSAMD21_ADC` and retain their original attribution in the source header. The RA8876 driver and the
bundled fonts retain their respective authors' notices.
