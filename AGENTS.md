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
differs: **7" = FT5206** (I²C 0x38), **10" = GT9271** (I²C 0x5D). Probe both I²C addresses at startup
to auto-detect which panel is fitted (see `GEVINO_TFT_Test`).

The complete pin map is in [README.md](README.md) and in [`src/gevino_tft_io.h`](src/gevino_tft_io.h).

## Golden rules

1. **Write non-blocking, state-based code.** Prefer `switch`/`case` state machines. Avoid `delay()`
   in `loop()`; use `millis()` or the bundled `Timer`.
2. **Respect the include order:**
   ```cpp
   #include "gevino_tft_io.h"    // board I/O, ADC, Timer
   #include "All_76_Pic_65K.h"   // image data (uses the `code`/PROGMEM macro)
   #include "gevino_tft_display.h"
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

Both controllers are on I²C (`Wire`). Detect the panel by probing both addresses:
- 7": FT5206 at `0x38`.
- 10": GT9271 at `0x5D` (INT on D38 / `TouchInt`).

Auto-detect at startup and use the matching driver for the rest of the program (see
`GEVINO_TFT_Test` and `Multi_Touch`).

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
