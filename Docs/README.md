# 📘 STM32 Smart Sensor Hub  
### A Modular, Power-Aware Firmware System with CLI Dashboard, Logging Framework, and Sensor Abstraction  

**Target MCU:** STM32 Nucleo-F446RE  
**Toolchain:** STM32CubeIDE / arm-none-eabi-gcc  
**UART Interface:** ST-LINK VCP (typically USART2 @ 115200)

---

## 🚀 Overview

The **STM32 Smart Sensor Hub** is a professionally structured embedded firmware project built to demonstrate modern firmware engineering practices:

- Modular layered architecture  
- Cooperative task scheduler  
- Runtime-configurable structured logging framework  
- Interactive UART CLI (dashboard-style)  
- Power management system  
- Sensor abstraction (simulated backend + real I²C/SPI sensor backends)  
- **Dynamic sensor sampling period that adapts to the current power mode**  
- GitHub Actions CI (compile-only)  
- Full documentation  
- Clean, maintainable C code with Doxygen-ready comments  

This repository is structured like a real production firmware codebase.

---

## 🏗️ Architecture at a Glance

```text
stm32-smart-sensor-hub/
├── Core/                     # Cube-generated startup, HAL, clock config
├── Drivers/                  # HAL drivers
├── app/                      # Application entry, task manager, main loop
├── common/                   # Logging framework, CLI, utilities
├── sensors/                  # Sensor abstraction + simulated sensor
├── power/                    # Power manager module
├── docs/                     # Documentation for all phases
└── .github/workflows/        # GitHub Actions CI
```

### Layer Summary

| Layer      | Purpose                                                        |
|------------|----------------------------------------------------------------|
| **Core/**  | System startup, HAL initialization, ISR handlers               |
| **app/**   | Main application logic + cooperative task scheduler            |
| **common/**| Logging subsystem, CLI interpreter, helpers                    |
| **sensors/** | Sensor interface API + simulated sensor backend             |
| **power/** | Power mode manager (Active/Idle/Sleep/Stop)                    |
| **docs/**  | Architecture docs, release notes                               |
| **.github/** | CI pipeline (ARM GCC build verification)                    |

---

## 🧩 Key Features

### ✔️ Cooperative Task Scheduler  

Tasks include:

- `Heartbeat`  
- `SensorSample`  
- `PowerManager`  
- `CLI`  

Each task has its own period and logs its execution timing.

---

### ✔️ Structured Logging Framework  

Log format:

```text
[00123456 ms][INF][../app/app_main.c:152][App_TaskSensorSample] T=26.1 C, P=101325 Pa, H=45.0 %RH
```

Supports:

- DEBUG / INFO / WARN / ERROR  
- Runtime filter control  
- CLI-controlled pause/resume  
- Automatic CLI prompt redraw after each log line  

---

### ✔️ UART CLI Dashboard  

Example commands:

```text
help
log pause
log resume
log info
log debug
pmode idle
status
```

Features:

- Non-blocking input  
- Backspace support  
- Dashboard-style, always-visible prompt  
- Logs never overwrite the CLI command entry line  

---

### ✔️ Sensor Abstraction Layer  

```c
typedef struct {
    bool (*init)(void);
    bool (*read)(SensorData_t *out);
} SensorIF_t;
```

Currently supports both simulated and hardware backends through the same interface:

- Simulated temperature sensor (`sensor_sim_temp.c/.h`) for early bring-up
- TSL2591 light sensor (I²C)
- BME280 environmental sensor (SPI2)

Additional sensors can be added without changing application code by implementing the same interface.---

### ✔️ Power Management Framework + Dynamic Sampling

Power modes:

- `POWER_MODE_ACTIVE`
- `POWER_MODE_IDLE`
- `POWER_MODE_SLEEP`
- `POWER_MODE_STOP`

The **SensorSample** task period automatically adapts to the current power mode:

```c
#define SENSOR_PERIOD_ACTIVE_MS   (1000U)   // 1 second in ACTIVE mode
#define SENSOR_PERIOD_IDLE_MS     (5000U)   // 5 seconds in IDLE mode
#define SENSOR_PERIOD_SLEEP_MS    (30000U)  // 30 seconds in SLEEP mode
#define SENSOR_PERIOD_STOP_MS     (0U)      // 0 => no sampling in STOP
```

- In **STOP** mode, sensor sampling is completely disabled.  
- The CLI `status` command reports the **effective sensor sample period**.

This models how a real low-power system would adapt workload based on its power budget.

---

### ✔️ GitHub Actions CI

- Installs ARM GCC  
- Builds firmware on Ubuntu  
- Verifies compilation on every push & PR  

---

## 🛠️ Build & Flash Instructions

1. Open the project in **STM32CubeIDE**.  
2. Build the project (Debug or Release).  
3. Flash the firmware to the **Nucleo-F446RE**.  
4. Open a serial monitor at **115200 baud**, 8N1.  
5. Reset the board and look for the CLI banner.  
6. Type `help` to see available commands.

---

## 🖥️ Example CLI Session

```text
Smart Sensor Hub CLI ready.
Type 'help' for commands.

> log info
[00001234 ms][INF][../app/app_main.c:129][App_TaskHeartbeat] Heartbeat task toggled LED
[00002234 ms][INF][../app/app_main.c:152][App_TaskSensorSample] SensorSample: value=23.10 C, timestamp=2234 ms

> pmode sleep
Requested power mode change: sleep

> status
Status:
  Task logging: ENABLED
  LogLevel: 1 (0=DEBUG,1=INFO,2=WARN,3=ERROR)
  PowerMode: 2 (0=ACTIVE,1=IDLE,2=SLEEP,3=STOP)
  Sensor sample period: 30000 ms
```

---

## 📄 Documentation

Included in `/docs`:

- **README.md** (this file)  
- **ARCHITECTURE.md** (layered design)  
- **RELEASE_NOTES.md** (per-phase changes)  
- **CLI_COMMANDS.md** (detailed CLI behavior and examples)  

All are updated at each project phase.

---

## 📌 Current Version: v0.5.0

Includes:

- CLI dashboard (UART) with clean prompt redraw
- Runtime log level control + pause/resume
- Cooperative task scheduler
- Power manager with **power-mode-dependent sampling**
- Sensor abstraction layer
- **TSL2591 (I²C) light sensor backend**
- **BME280 (SPI2) environmental sensor backend**
- GitHub Actions CI (compile-only)
- Updated documentation

---

## 🧭 Roadmap (Planned Phases)

- Phase 6 → Real STM32 low-power mode entry/wakeup  
- Phase 6 → Real STM32 low-power mode entry/wakeup  
- Phase 7 → Event/state machine on top of tasks  
- Phase 8 → Flash/SD logging or host-side tooling  
- Phase 9 → BLE or USB link to external dashboard  
- Phase 10 → Remote telemetry / visualization  

---

## 💼 Resume Impact

This project demonstrates:

- Embedded architecture & modular design  
- Reusable driver and sensor abstraction  
- Task scheduling and power management  
- **Power-aware behavior (dynamic sensor sampling vs power mode)**  
- CLI protocol handling and UX considerations  
- Logging frameworks with runtime control  
- CI/CD discipline for embedded firmware  

Ideal to showcase for **Embedded/Firmware Engineer** roles.

---

## 📝 License

MIT
