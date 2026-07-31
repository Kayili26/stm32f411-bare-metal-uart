# Bare-Metal USART2 Driver & Interactive Command Shell
**Target Hardware:** ST Nucleo-F411RE (ARM Cortex-M4 @ 16 MHz HSI)  
**Development Paradigm:** Pure Bare-Metal C (Direct Register Access). No HAL, LL, or CubeMX auto-generation.

---

## Technical Overview
This project implements a monolithic, polling-driven USART2 peripheral driver and an interactive command-line interface (CLI) shell for the STM32F411CE/RE MCU. All operations are written from scrat[...]

### System Architecture & Configuration
* **Clock Tree:** System runs on the internal 16 MHz High-Speed Internal (HSI) oscillator. APB1 and AHB1 bus prescalers are left at default values ($HCLK = PCLK1 = 16\text{ MHz}$).
* **Gating:** Explicit peripheral clock enablement via `RCC_AHB1ENR` (Bit 0 for GPIOA) and `RCC_APB1ENR` (Bit 17 for USART2).
* **Pin Multiplexing:** `PA2` (TX) and `PA3` (RX) are routed to Alternate Function 7 (`AF07`) via `GPIOA_AFRL` fields to interface with the embedded ST-LINK Virtual COM Port.
* **Baud Rate Generation:** Configured for **115200 8N1**. The Baud Rate Register (`USART2_BRR`) is loaded with `0x8B` (Fractional baud rate calculation: $\frac{16,000,000}{16 \times 115200} = 8.6[...]

---

## Hardware Architecture
* **Development Board:** ST Nucleo-F411RE
* **Interfacing:** Integrated ST-LINK V2-1 Virtual COM Port over USB.
* **Debug Pins:** `PA2` (USART2_TX), `PA3` (USART2_RX), and `PA5` (User LED1).
* **Instrumentation:** 24 MHz 8-channel logic analyzer hooked into TX/RX test points for hardware-level protocol decoding.

---

## Firmware Features & Shell Capabilities
* **Zero-Abstraction GPIO Driver:** Explicit masking and bitwise operations on `GPIOA_MODER` and `GPIOA_ODR` to toggle the user LED without runtime abstraction penalties.
* **Polled Transmit/Receive State Machine:** Code blocks deterministically on the `TXE` (Transmit Data Register Empty) and `RXNE` (Receive Data Register Not Empty) status flags in `USART2_SR`.
* **Interactive CLI Shell:** Supports line-buffered processing with dynamic memory tracking up to a 32-byte threshold.
  * **Command Parser:** Evaluates string literals via strict boundary checks supporting: `led on`, `led off`, and `status`.
  * **UX Utilities:** Full destructive backspace support (`0x7F` / `\b`) handling physical terminal cursor repositioning and active local echo.

---

## Verification
Functional verification via serial terminal:
- Full printable ASCII range (0x20–0x7E) transmitted and received without corruption
- Baud sensitivity confirmed: correct output at 115200, garbage at 9600,
  validating the computed BRR value (0x8B from 16 MHz APB1 clock)
- Bidirectional: shell commands parsed correctly, responses match LED state
- Sustained transmission over several minutes with no dropped bytes

Planned: signal-level capture of the TX line with a logic analyzer
(start/stop bit timing, UART frame decode at 115200 baud).

## Key Engineering Takeaways
Developing this driver provided deep practical insight into the exact synchronization required between peripheral clocks and core logic; missing an explicit RCC enable bit results in a hard fault [...]

---

## Screenshot
![PuTTY terminal showing the UART command shell](Screenshot%202026-07-30%20172729.png)

Caption: PuTTY connected to the Nucleo‑F411RE running the interactive UART shell (115200 8N1). Try commands: `status`, `led on`, `led off`.

Alt-text: PuTTY serial terminal window connected to an ST Nucleo‑F411RE running the bare‑metal USART2 command shell at 115200 8N1. The terminal shows line‑buffered input with local echo and the MCU's responses to commands such as 'status', 'led on', and 'led off', confirming LED state changes.
