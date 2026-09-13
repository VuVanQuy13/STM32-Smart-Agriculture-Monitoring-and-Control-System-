# STM32 Smart Agriculture Monitoring and Control System

Bare-metal C firmware for STM32F103 with an 8 MHz external crystal and a 72 MHz
system clock. Uses register-level drivers, custom startup code and a linker
script, without HAL or an RTOS.

## Functionality

- DHT11 temperature/humidity and ADC soil/light acquisition every ten seconds.
- Two 16x2 I2C LCDs for readings and editable thresholds.
- Debounced buttons and Auto/Manual pump, light and fan relay control.
- Threshold persistence using two Flash pages, CRC and commit verification.
- Sensor error/age checks, communication timeouts and a RAM fault log.

## Layout

| Path | Purpose |
| --- | --- |
| Core/main.c | Firmware application entry point |
| Core/Test.cpp | Historical Arduino reference, not built or authoritative wiring |
| App/ | Sensor, display, button, relay and configuration logic |
| Driver/ | Register definitions and peripheral drivers |
| Startup/, Linker/ | Reset/vector table and memory layout |
| Tests/ | Deterministic host tests and build/linker checks |
| Tools/ | Build configuration tracking helper |

## Build and Test

The current build scripts require Windows PowerShell, GNU Make and GNU Arm GCC.
Host tests additionally require a native `gcc` on PATH. Run from this directory:

```powershell
make build GCC_DIR=C:/path/to/arm-toolchain
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/run.ps1 -Optimization -O0
powershell -NoProfile -ExecutionPolicy Bypass -File Tests/run.ps1 -Optimization -O2
```

`GCC_DIR` must contain `bin/arm-none-eabi-gcc` and `bin/arm-none-eabi-objcopy`.
The default compiler path in Makefile is local to the original development PC.
Firmware output is `Output/makefile.hex`; `Output/` is generated and ignored.
Use `OPT=-O2 OUTPUT_DIR=Output/optimized` for an optimized variant.
`Tests/check_build.ps1` uses the default Makefile toolchain configuration;
`Tests/check_linker.ps1` also has a local compiler path to adjust on another PC.

## Hardware and Limits

- DHT11: PA0; light ADC: PA5; soil ADC: PA6.
- Active-high relays: PA2 pump, PA3 light, PA4 fan.
- I2C1: PB6 SCL, PB7 SDA; LCD addresses 0x27 and 0x26.
- Actual button mapping is in App/Src/button.c; SWD remains enabled, JTAG is disabled.
- Flash layout targets 64 KB: 62 KB application plus 2 KB configuration; RAM is 20 KB.
- Low soil ADC means low displayed percentage and requests watering. Percentages
  are ADC full-scale ratios, not calibrated physical moisture measurements.
- The firmware has no independent pump maximum-run protection or watchdog recovery.
  A plausible but incorrect low soil reading can keep the pump on. Manual mode
  leaves output control with the user. LCD errors latch until reboot; LCD2 errors
  disable threshold editing. Millisecond timing can undercount during blocked IRQs.
- Host tests do not replace board validation of wiring, DHT timing, power loss,
  sensor calibration and relay-load interference. Validate before unattended use.
