# Curriculum — ESP32 Thermal Monitor for SCM Tech Z1 Spindle

Modules map 1:1 to build phases. Work them in order; each module lists its micro-units (15–30 min each), the concepts to teach, depth notes, and the quiz pool for spaced repetition. Interview-ready depth throughout; go deeper on FreeRTOS/ESP32 internals and embedded C++/memory (marked ★).

---

## Module 0 — Toolchain & First Flash
*Build phase: blink the LED.*

**Units:**
1. **What PlatformIO actually does** — build system vs IDE, platformio.ini anatomy, where the toolchain lives, what `framework = arduino` pulls in (Arduino as a layer over ESP-IDF ★).
2. **From source to silicon** — compile → link → .bin, what the partition table is, what "flashing" physically does, why the board auto-resets (DTR/RTS trick), bootloader stages of the ESP32 ★.
3. **Serial/UART** — what UART is at signal level, baud rate, why 115200, the USB-to-serial chip's role.

**Socratic anchors:** Why does the upload sometimes need the BOOT button held? What's actually stored in flash vs RAM when the chip boots?

**Quiz pool:** Explain the ESP32 boot sequence to first line of `setup()`. What's in platformio.ini and why does the board setting matter? UART vs USB — what converts between them on your board?

---

## Module 1 — Electricity for the Project
*Build phase: before wiring anything.*

**Units:**
1. **V, I, R as used here** — voltage rails (3.3 vs 5V), why the ESP32 GPIO is 3.3V-only, current draw and budgets (~250mA total), reading a multimeter safely.
2. **Decoupling & pullups** — why breakouts have capacitors, what a pullup resistor does and why I2C needs them (open-drain ★ preview), where the pullups are on his breakouts.
3. **Power on the Z1** — 24V control rail vs mains, why you never tap 400V, buck vs linear regulation, what the LM2596 does (switching, efficiency, ripple).

**Quiz pool:** Why is connecting a 5V signal to an ESP32 GPIO bad, exactly what fails? Your buck is set to 5V — why does the ESP32 board accept that on VIN but not on the 3V3 pin? What is ripple and why does the LM2596 spec say input must be 1.5V above output?

---

## Module 2 — I2C & the OLED
*Build phase: text on the SSD1306.*

**Units:**
1. **I2C at the signal level** — SDA/SCL, open-drain + pullups (now in full), start/stop conditions, addresses, ACK/NACK, clock speed. Draw/trace a transaction.
2. **Addressing & the bus** — 7-bit addresses, how two devices share the bus, address conflicts, I2C scanner (he writes one — this is the exception where it's small enough to be a guided exercise, not given).
3. **SSD1306 internals** — framebuffer concept (ties to his rasterizer!), pages and segments, why the library keeps a 1KB buffer in RAM ★, `display()` as a buffer flush over I2C.
4. **The Wire library & ESP32 I2C peripheral** — hardware I2C controllers, pin muxing (any-pin I2C via GPIO matrix ★), default pins 21/22.

**Socratic anchors:** His rasterizer has a framebuffer — same idea here, what's different at 1 bit per pixel? Predict: how long does flushing 1KB at 400kHz take? (Then measure.)

**Quiz pool:** Walk through one complete I2C write transaction line-state by line-state. Why open-drain instead of push-pull for a shared bus? Bus works at 10cm, fails at 50cm — why, two fixes (interview style). How does the ESP32 route I2C to arbitrary pins?

---

## Module 3 — The MLX90640
*Build phase: real temperatures over serial.*

**Units:**
1. **How an IR thermal array works** — microbolometers/thermopiles, why it needs per-pixel calibration, emissivity (critical: sensor points at tool-material contact zone, ~40cm away, 55° FoV → ~1.3cm×1cm per pixel. Wood/MDF workpiece ~0.9 emissivity = accurate. Cutting tool is polished carbide/HSS = low emissivity ~0.3, will underread — measuring chip/workpiece surface near cut is more reliable. Fire risk from material, tool wear from heat — both at the contact zone).
2. **Datasheet reading as a skill** — find together: I2C address, refresh rate register, RAM layout, supply specs. Goal: he navigates the datasheet, not memorizes it.
3. **The driver pipeline** — EEPROM calibration extraction, raw frames → `MLX90640_CalculateTo()`, why it returns 768 floats, subpage/chess-pattern reading ★.
4. **Memory & float cost ★** — 768 floats = 3KB per frame, stack vs heap vs static placement, what the FPU on the ESP32 does and doesn't accelerate (single precision only), measuring compute time per frame.
5. **I2C at 400kHz+ and clock stretching** — why this sensor pushes the bus, frame rate vs bus bandwidth math.

**Quiz pool:** Why does each MLX90640 unit need calibration data from its own EEPROM? Frame-rate-vs-bandwidth: at 8Hz, how much I2C traffic per second, roughly? Where should the 768-float frame buffer live in memory and why (interview style ★)? What's emissivity and how does it bite you when pointing at a polished carbide tool vs the wood workpiece surface?

---

## Module 4 — Heatmap Rendering
*Build phase: the fun part. Strong overlap with his graphics background — lean on it.*

**Units:**
1. **Mapping 32×24 → 128×64** — nearest neighbor vs bilinear interpolation (he knows barycentric — bilinear is a gift here), scaling math.
2. **Temperature → intensity on a mono display** — normalization, min/max vs fixed range, dithering patterns (Bayer) to fake grayscale on 1-bit pixels.
3. **Fixed-point arithmetic ★** — when float is too slow/big, Q-format, converting his pipeline, measuring the difference. Interview classic.
4. **Frame timing** — measuring ms per stage (read/compute/render/flush), where the bottleneck is, double-buffering concepts.

**Quiz pool:** Implement-in-words bilinear interpolation for one output pixel. Why might fixed-point beat float here even though the ESP32 has an FPU ★? Your loop runs at 4Hz but sensor refreshes at 8Hz — what are the candidate bottlenecks and how do you find which?

---

## Module 5 — Architecture & FreeRTOS ★★
*Build phase: restructure from loop() spaghetti into tasks. His #1 chosen focus area — go deep.*

**Units:**
1. **Why an RTOS** — superloop vs tasks, what Arduino's `loop()` actually is on ESP32 (a FreeRTOS task already!), preemption, the tick.
2. **Tasks & scheduling** — `xTaskCreate`, priorities, the two cores ★ (PRO_CPU/APP_CPU, what runs where, `xTaskCreatePinnedToCore`), stack sizing per task and what stack overflow looks like.
3. **Queues & sharing data** — why globals + flags break, queues for sensor→render handoff, mutexes for the I2C bus, priority inversion (interview classic).
4. **ISRs & timing** — what can/can't run in an ISR, `IRAM_ATTR`, debouncing if he adds a button, hardware timers.
5. **Watchdogs & robustness** — task watchdog, brownout detector, what resets the chip in a shop environment and how to log/recover.
6. **ESP32 memory map ★** — IRAM/DRAM, flash-mapped code, why PSRAM on his WROVER exists, heap caps, `heap_caps_malloc`.

**Restructure exercise:** sensor-read task → queue → render task, mutex on Wire. He designs it on paper first, defends the priorities chosen, then implements.

**Quiz pool (rich — this is interview gold):** What happens when two tasks call `Wire` simultaneously without a mutex — describe an actual failure trace. Explain priority inversion and one mitigation. Your render task overflows its stack — symptoms, diagnosis, fix. Why pin WiFi work to core 0? Difference between queue and global-with-mutex, when each. What is the task watchdog and why might `loop()` starvation trip it?

---

## Module 6 — Alerts & Spindle Logic
*Build phase: make it actually useful at work.*

**Units:**
1. **Thresholds done right** — hysteresis (why a single threshold chatters), moving averages/filtering noisy IR data, rate-of-rise as an earlier warning than absolute temp.
2. **State machines ★ (embedded C++ patterns)** — enum class + switch vs table-driven, modeling NORMAL/WARM/ALERT/FAULT, why explicit state machines are the embedded idiom.
3. **Domain calibration** — sensor points at tool-material contact zone (not bearing). Establish baseline temps for normal cuts per material/tool/RPM/feed combo. Emissivity correction: use ~0.9 for wood surface readings. Rate-of-rise on the hotspot pixel cluster is the early fire warning signal; absolute threshold for tool wear correlation.

**Quiz pool:** Why hysteresis — show the chattering failure without it. Sketch the alert state machine, defend each transition. How would you distinguish a real temperature trend from ambient shop temperature rising on a hot day?

---

## Module 7 — Deployment & Power
*Build phase: onto the machine.*

**Units:**
1. **24V tap in practice** — verifying the rail with the multimeter (procedure!), fusing the tap, wire gauge, where to physically tap safely.
2. **EMI & the shop ★ (industrial angle)** — what the VFD/spindle drive radiates, why I2C wires must be short, twisted pairs, shielding, ferrites, brownout symptoms vs EMI symptoms.
3. **Mechanical** — vibration vs solder joints vs breadboards (breadboard dies here — perfboard soldering unit), strain relief, enclosure & sensor window considerations (IR doesn't pass regular glass!).
4. **Field-debugging** — serial is gone now: LED blink codes, logging to flash, remote logging preview.

**Quiz pool:** Your device resets randomly only when the spindle runs — give three hypotheses ordered by likelihood and a test for each (this is THE interview question your project earns you). Why can't the MLX90640 look through a normal acrylic window?

---

## Module 8 — ESP32 Telemetry over WiFi ★
*Build phase: the device starts talking to the network. Gateway to the web app extension (Modules 9–11).*

**Units:**
1. **WiFi on ESP32 ★** — what the WiFi stack costs (RAM, core 0 time), station mode, reconnect handling as a state machine (Module 6 callback), RSSI in a metal-filled shop.
2. **MQTT vs HTTP for telemetry** — pub/sub model, why MQTT fits sensor streams, QoS levels, topic design (`z1/cut/temp`, `z1/cut/status`), retained messages.
3. **Serializing on-device ★** — JSON payloads with ArduinoJson, its memory model (stack vs heap documents), payload size vs publish rate budgeting, binary vs JSON tradeoff.
4. **Timestamps & NTP** — why device time matters for correlation later, NTP sync, what happens to timestamps when WiFi drops (buffer? drop? mark unsynced?).
5. **OTA updates** — partition schemes revisited (Module 0 callback), why OTA needs two app partitions, rollback.
6. **Watchdog interaction ★** — WiFi task starvation, the classic "added WiFi and now it resets" failure, pinning and priorities revisited (Module 5 callback).

**Quiz pool:** Why does adding WiFi sometimes cause task watchdog resets ★? MQTT QoS 0/1/2 — which for live temps vs alert events, and why different? Your publish task needs a 2KB JSON buffer — where do you allocate it and why ★? Walk through what happens in flash during an OTA update. WiFi drops for 90 seconds mid-run — design the device-side behavior so the run log stays trustworthy.

---

## Module 9 — The Shop Server: Broker, Database, Backend
*Build phase (web app extension): a local machine in the shop receives, stores, and serves the data. Python backend — leans on his existing Python skills.*

**Units:**
1. **Architecture on a whiteboard first** — ESP32 → Mosquitto (MQTT broker) → ingestion service → SQLite → API → browser. Why each box exists; what breaks if you skip the broker.
2. **Mosquitto** — install, config, auth basics, testing with `mosquitto_sub` before any code exists (incremental bring-up, the embedded habit applied to infra).
3. **Data modeling** — `runs` table (id, start/end, tool, spindle RPM, feed rate, material, notes) and `samples` table (timestamp, min/max/avg temp, hotspot coords, run_id nullable FK). Why store aggregates per frame, not all 768 pixels — data volume math. Schema as a designed thing, not an accident.
4. **The ingestion service** — Python MQTT client (paho-mqtt) subscribing and writing to SQLite, batching writes, what happens at 8 samples/sec for an 8-hour shift (back to the volume math).
5. **A small API** — FastAPI or Flask: endpoints for live latest-value, run start/stop, run metadata entry, historical queries. REST conventions just deep enough to be conversant.
6. **Running it for real** — systemd service, the machine's static IP or mDNS (`shopserver.local`), surviving reboots, where logs go.

**Quiz pool:** Why a broker between device and database instead of the ESP32 writing to the DB directly? Defend the two-table schema — what query becomes painful if samples and runs are one table? 4Hz sampling, 8h shift: how many rows, roughly what size — and at what point would SQLite stop being the right answer? Your ingestion service crashed for an hour; the broker was up — what determines whether data was lost?

---

## Module 10 — The Web App: Live View & Run Logging
*Build phase (web app extension): the browser side. Plain HTML/JS first — no framework until there's a reason.*

**Units:**
1. **Live data into a browser** — polling vs Server-Sent Events vs WebSockets, picking SSE for simplicity, wiring the API's latest-value into a live readout.
2. **Charting time series** — a lightweight chart lib (uPlot or Chart.js), rolling live window, rendering performance basics (his graphics instincts apply: what's actually being redrawn?).
3. **The run workflow** — Start Run form (tool, spindle speed, feed rate, material, notes) → server marks samples with run_id → Stop Run. Designing the UX around dusty-shop reality: big buttons, tablet-friendly, works when his hands are full.
4. **Browsing history** — run list, per-run detail page with temperature curve, annotating runs after the fact.
5. **(Optional) live heatmap in the browser** — publish the full 768-pixel frame on demand to a canvas-rendered heatmap. Pure fun, pure graphics-background flex.

**Quiz pool:** SSE vs WebSocket vs polling — tradeoffs, and why the choice here. The live chart stutters as the window grows — three candidate causes in order of likelihood. Where should run start/stop state live: device, server, or browser — defend it (this one has teeth).

---

## Module 11 — Correlation: Making the Data Mean Something
*Build phase (web app extension): the payoff — tool/RPM/feed vs temperature. This is the part that makes machinists and interviewers both lean in.*

**Units:**
1. **Joining metadata to time series** — per-run aggregates (peak temp, time-to-peak, settling temp), SQL GROUP BY against the run metadata.
2. **Comparisons that answer shop questions** — same tool at different RPMs, same RPM with different feeds, temp-rise rate as the early-warning signal (Module 6 callback). Plotting these in the app.
3. **Honest data reading** — confounders (ambient shop temp, run duration, duty cycle), why n=3 runs proves little, baseline drift. Just enough rigor to not fool himself.
4. **Export & write-up** — CSV export endpoint, and a short findings write-up: this becomes the portfolio README's centerpiece ("measured X, found Y").

**Quiz pool:** Peak temp vs temp-rise rate — which correlates better with a dulling tool, and how would you actually test that? Two runs, same tool, 10°C apart at peak — list the confounders before blaming the parameters. Design the SQL (in words) for "average peak temp per tool, ordered worst-first."

---

## Module 12 — Interview Consolidation
*When the build is done (with or without the web extension).*

- He writes (himself) a one-page architecture description of the full system — device, transport, server, frontend — then defends it against your questioning.
- Full-system quiz drawing from all modules' pools, interview-style.
- "Tell me about a hard bug" — rehearse 2–3 real war stories from this build with the STAR-ish structure interviewers expect.
- Map every module to job-ad keywords (I2C, FreeRTOS, embedded C++, debugging, power, EMI, MQTT, REST, SQL, full-stack telemetry) so he can speak to each with a concrete story.
- If the web extension was built: rehearse the end-to-end story — "data is born in an IR sensor and ends as a chart that changed how we run a machine." That sentence is the whole pitch.
