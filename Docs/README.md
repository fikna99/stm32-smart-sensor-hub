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
- Simulated sensor abstraction (real sensors later)  
- GitHub Actions CI (compile-only)  
- Full documentation  
- Clean, maintainable C code with Doxygen-ready comments  

This repository is built just like a real production firmware codebase.

## 🚀 Current Status (Phase 2)

- **Logging subsystem** over UART2 with timestamps and metadata.
- **Cooperative Task Manager** for periodic task execution.
- **Heartbeat Task** toggling the on-board LED and logging activity.
- **Generic Sensor Interface** abstraction (`SensorIF_t`, `SensorData_t`).
- **Simulated Temperature Sensor** generating a smooth sine-wave signal.
- **Sensor Sampling Task** running every 1 second and logging structured readings.
---

## 🏗️ Architecture at a Glance

```
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

| Layer | Purpose |
|-------|---------|
| **Core/** | System startup, HAL initialization, ISR handlers |
| **app/** | Main application logic + cooperative task scheduler |
| **common/** | Logging subsystem, CLI interpreter, helpers |
| **sensors/** | Sensor interface API + simulated sensor backend |
| **power/** | Power mode manager (Active/Idle/Sleep/Stop) |
| **docs/** | Architecture docs, release notes |
| **.github/** | CI pipeline (ARM GCC build verification) |

---

## 🧩 Key Features

### ✔️ Cooperative Task Scheduler  
Tasks include:

- `Heartbeat`  
- `SensorSample`  
- `PowerManager`  
- `CLI`  

Each task has its own period and logs its timing.

---

### ✔️ Structured Logging Framework  

Format:

```
[00123456 ms][INF][../app/app_main.c:152][App_TaskSensorSample] Value=23.1C
```

Supports:

- DEBUG / INFO / WARN / ERROR  
- Runtime filter control  
- CLI-controlled pause/resume  
- Auto-restores CLI prompt after each log line  

---

### ✔️ UART CLI Dashboard  

Commands:

```
help  
log pause  
log resume  
log info/debug/error  
pmode idle/sleep/stop  
status
```

Features:

- Non-blocking input  
- Backspace support  
- Dashboard-style always-visible prompt  
- Logs never overwrite CLI command entry  

---

### ✔️ Sensor Abstraction Layer  

```
typedef struct {
    bool (*init)(void);
    bool (*read)(SensorData_t *out);
} SensorIF_t;
```

Currently uses a simulated temp sensor.  
Later you can plug in:

- I²C sensor  
- SPI sensor  
- ADC sensor  
- Virtual sensor for testing  

---

### ✔️ Power Management Framework  

Tracks:

- power mode  
- idle cycles  
- transition logs  

Future versions will include **real STM32 low-power mode entry**.

---

### ✔️ GitHub Actions CI

- Installs ARM GCC  
- Builds firmware  
- Verifies compilation on every push & PR  

---

## 🛠️ Build & Flash Instructions

1. Open in STM32CubeIDE  
2. Select **Build**  
3. Debug → Flash  
4. Open serial monitor @ 115200 baud  
5. Type `help` to see commands  

---

## 🖥️ Example CLI Session

```
Smart Sensor Hub CLI ready.
Type 'help' for commands.

> log info
[00001234 ms][INF][Heartbeat] LED toggled
[00002234 ms][INF][SensorSample] Value=23.1 C
>
```

Pause logs to inspect:

```
> log pause
```

---

## 📄 Documentation

Included in `/docs`:

- **README.md** (root description)  
- **ARCHITECTURE.md**  
- **RELEASE_NOTES.md**  

All are auto-updated each phase.

---

## 📌 Current Version: v0.3.0

Includes:

- CLI dashboard  
- Logging framework enhancements  
- Power Manager foundation  
- Task Manager improvements  
- GitHub workflow  
- Full documentation  

---

## 🧭 Roadmap

- Phase 4 → Real I²C sensor integration  
- Phase 5 → Real STM32 low-power mode  
- Phase 6 → Event/state machine  
- Phase 7 → Flash/SD logging  
- Phase 8 → BLE or USB layer  
- Phase 9 → External wireless dashboard  

---

## 💼 Resume Impact

This project demonstrates:

- Embedded architecture  
- Modular C design  
- Reusable driver abstraction  
- Task scheduling  
- Power management  
- CLI protocol handling  
- Logging frameworks  
- CI/CD discipline  

Ideal for Embedded/Firmware engineer roles.

---

## 📝 License

MIT (pending your choice).
