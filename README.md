# STM32 Agriculture Monitoring and Control System

An STM32 project that monitors temperature, air humidity, soil moisture and light,
and controls a pump, light and fan through relays. The firmware is written in
bare-metal C using direct register access, without HAL or an RTOS.

## Features

- Read sensors every 10 seconds and display readings on an LCD.
- Automatically control the pump, light and fan using configurable thresholds.
- Switch to Manual mode to turn each device on or off using buttons.
- Edit thresholds, save settings to Flash and restore default thresholds.

## MCU, Sensors and Devices

| Component | Purpose |
| --- | --- |
| STM32F103C8T6 — ARM Cortex-M3 | Main MCU, running at 72 MHz from an external 8 MHz crystal |
| DHT11 | Measure temperature and air humidity |
| Analog soil moisture sensor | Provide soil readings through the ADC |
| Analog light sensor | Provide light readings through the ADC |
| Two 16×2 I2C LCDs | Display sensor readings and configured thresholds |
| Three relays | Control the pump, light and fan |
| Buttons and LEDs | Adjust settings, control devices manually and indicate status |

Displayed soil and light percentages are ratios of the ADC full scale, rather
than calibrated measurements of physical soil moisture or illuminance.

## MCU Peripherals

- **RCC:** configure the system clock and enable peripheral clocks.
- **GPIO, AFIO and EXTI:** connect sensors, relays and LEDs, and process button input.
- **ADC1:** read the light sensor on PA5 and soil sensor on PA6.
- **I2C1:** communicate with the LCDs at addresses `0x27` and `0x26` using PB6/PB7.
- **TIM2, TIM3, TIM4 and SysTick:** generate delays, schedule sampling, measure DHT11 pulses and debounce buttons.
- **Internal Flash:** preserve configured thresholds across restarts.

## Circuit Schematic

![Circuit schematic](https://github.com/user-attachments/assets/fba3842c-e7d0-4d2a-ae84-66889b4d4f94)

## PCB Layout

![PCB layout](https://github.com/user-attachments/assets/91b17522-d36f-44c0-847f-05722fea052f)

## Build

Requirements: **Windows PowerShell**, **GNU Make** and **GNU Arm GCC**.
Make sure `powershell` and `make` are available on your PATH.

Open a terminal in the project root directory, which contains `Makefile`, and run:

```powershell
make build GCC_DIR=C:/Toolchains/arm-gnu-toolchain
```

`make build` compiles and links the firmware. `GCC_DIR` specifies your ARM
toolchain installation directory; the path above is only an example. Replace it
with the directory containing `bin/arm-none-eabi-gcc` and
`bin/arm-none-eabi-objcopy`, and use a path without spaces.

For example, if your compiler is `D:/ARM_GCC/bin/arm-none-eabi-gcc.exe`, run:

```powershell
make build GCC_DIR=D:/ARM_GCC
```

A successful build creates **`Output/makefile.hex`**. This command generates the
firmware file; it does not flash the STM32 board.
