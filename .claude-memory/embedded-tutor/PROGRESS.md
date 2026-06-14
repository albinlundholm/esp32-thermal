# PROGRESS — ESP32 Thermal Monitor Curriculum

> Dates in YYYY-MM-DD.

## Current position

- **Module:** 4
- **Unit:** 4
- **Next up:** Module 4 Unit 4 — Frame timing (measure ms per stage with micros(), find bottleneck). Blocked on hardware: MLX90640 needs soldering before wiring up. Once soldered, wire SDA→21, SCL→22, VIN→3.3V, GND→GND, run I2C scanner to confirm 0x33 appears alongside 0x3C, then implement sensor read pipeline.

## Module status

| Module | Status | Started | Completed |
|---|---|---|---|
| 0 — Toolchain & First Flash | complete | 2026-06-12 | 2026-06-12 |
| 1 — Electricity | complete (partial — multimeter + 24V tap deferred to deployment) | 2026-06-12 | 2026-06-12 |
| 2 — I2C & OLED | complete | 2026-06-13 | 2026-06-13 |
| 3 — MLX90640 | complete | 2026-06-13 | 2026-06-13 |
| 4 — Heatmap Rendering | in progress | 2026-06-13 | |
| 5 — FreeRTOS ★ | not started | | |
| 6 — Alerts & State Machines | not started | | |
| 7 — Deployment & Power | not started | | |
| 8 — WiFi Telemetry ★ | not started | | |
| 9 — Shop Server (broker/DB/API) | not started | | |
| 10 — Web App (live view/runs) | not started | | |
| 11 — Correlation & Analysis | not started | | |
| 12 — Interview Consolidation | not started | | |

## Spaced repetition log

Intervals: pass → +2d, +7d, +21d, retired. Fail/shaky → reset to +2d.

| Concept | Module | Last quizzed | Result | Next review due |
|---|---|---|---|---|
| ESP32 two-stage bootloader + DTR/RTS auto-reset | 0 | 2026-06-12 | pass | 2026-06-14 |
| compile→link→.bin→flash pipeline | 0 | 2026-06-12 | pass | 2026-06-14 |
| platformio.ini anatomy + framework=arduino meaning | 0 | 2026-06-12 | pass | 2026-06-14 |
| GPIO current limits + power vs signal pins | 1 | 2026-06-12 | pass | 2026-06-14 |
| I2C open-drain + pullups + parallel stacking | 1 | 2026-06-12 | pass | 2026-06-14 |
| I2C signal level: start/stop, address frame, ACK/NACK, R/W bit | 2 | 2026-06-13 | pass | 2026-06-15 |
| I2C scanner: address space, reserved addresses, Wire.endTransmission() return value | 2 | 2026-06-13 | pass | 2026-06-15 |
| SSD1306 framebuffer: 1-bit/pixel, 1KB buffer, pages/segments, display() as I2C flush, ~25ms per flush at 400kHz | 2 | 2026-06-13 | pass | 2026-06-15 |
| MLX90640: thermopiles, emissivity, per-pixel factory calibration from EEPROM | 3 | 2026-06-13 | pass | 2026-06-15 |
| MLX90640 datasheet navigation: I2C address 0x33, control reg 0x800D, refresh rate bits B7-B9, memory map | 3 | 2026-06-13 | pass | 2026-06-15 |
| Driver pipeline: EEPROM extraction → frame read (834 words, ~37ms at 400kHz) → CalculateTo() | 3 | 2026-06-13 | pass | 2026-06-15 |
| Stack vs static vs heap for embedded buffers; fragmentation risk; file-scope static pattern | 3 | 2026-06-13 | pass | 2026-06-15 |
| I2C signal integrity at high speed: RC rise time, wire capacitance, why 400kHz not 1MHz in shop | 3 | 2026-06-13 | pass | 2026-06-15 |
| Nearest neighbor vs bilinear for hotspot detection — NN sufficient for max-scan use case | 4 | 2026-06-13 | pass | 2026-06-16 |
| Bayer dithering: threshold matrix, tiling with % 4, normalize→scale→compare pipeline | 4 | 2026-06-13 | pass | 2026-06-16 |
| Fixed-point: Q8 format, uint32_t for intermediates, >> vs / for power-of-2 division | 4 | 2026-06-13 | pass | 2026-06-16 |

## Previewed ahead of sequence

| Concept | Date | Note |
|---|---|---|

## Session notes

| Date | What was covered | Notes |
|---|---|---|
| 2026-06-12 | Module 0 Unit 1: platformio.ini anatomy, platform vs framework, UART vs I2C, baud rate, I2C addressing, Wire object | Strong session — good instincts throughout |
| 2026-06-12 | Module 0 Unit 2: compile→link→.bin pipeline, partition table, flash layout, two-stage bootloader, DTR/RTS auto-reset trick, OTA preview. Wired SSD1306 over I2C, flashed and got Hello! on display. | First breadboard session — handled it well. Board is Olimex ESP32-WROVER-DEV-LiPo (updated platformio.ini board ID). PlatformIO installed via pipx. |
| 2026-06-12 | Module 1: 3.3V-only logic, GPIO current limits (12mA), power vs signal pins, open-drain + pullups, parallel pullup stacking. Skipped multimeter + 24V tap (deferred to deployment). | Good session. |
| 2026-06-13 | Module 2 Units 1–3: I2C signal level (start/stop, address frame, ACK/NACK, R/W bit), wrote I2C scanner from scratch (found SSD1306 at 0x3C), SSD1306 framebuffer internals (1-bit/pixel, pages/segments, display() flush cost). | Solid — good instincts. Debugged missing Serial.begin() and Wire.begin() independently. Added terminal/CLI as extra-focus area per user request. |
| 2026-06-13 | Module 2 Unit 4: Wire library, ESP32 GPIO matrix, pin muxing, two I2C controllers. Module 3 all units: thermopiles, emissivity (sensor will point at tool-material contact zone, not spindle bearing), datasheet navigation (0x33 address, 0x800D control reg, refresh rate bits B7-B9), driver pipeline, memory placement (stack/static/heap), I2C signal integrity at speed. | Fast session — moved through M3 quickly. Good datasheet navigation instincts. |
| 2026-06-13 | Module 4 Units 1–3: nearest neighbor vs bilinear (NN sufficient for hotspot use case), Bayer dithering pipeline, fixed-point arithmetic, bit-shift vs divide. Blocked on MLX90640 — needs pin header soldered before wiring. | Good session. Dithering was new concept, picked it up well. |
