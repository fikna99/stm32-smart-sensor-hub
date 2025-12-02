# Release Notes

## v0.4.0 – Dynamic Sensor Sampling Based on Power Mode

### Added

- **Power-mode-aware sensor sampling**
  - The `SensorSample` task now adjusts its period based on the current `PowerMode_t`.
  - New configuration constants:
    - `SENSOR_PERIOD_ACTIVE_MS   (1000U)`   – 1 second in ACTIVE mode
    - `SENSOR_PERIOD_IDLE_MS     (5000U)`   – 5 seconds in IDLE mode
    - `SENSOR_PERIOD_SLEEP_MS    (30000U)`  – 30 seconds in SLEEP mode
    - `SENSOR_PERIOD_STOP_MS     (0U)`      – 0 => no sampling in STOP
  - In `POWER_MODE_STOP`, sensor sampling is completely disabled.

- **Status reporting of sample period**
  - The `status` CLI command now reports the **effective sensor sample period** derived from the current power mode.

### Rationale

This phase links the abstract power management layer to real, observable
behavior in the system (sensor sampling frequency). It demonstrates how
embedded firmware can gracefully scale back activity as the system moves
into deeper power-saving states.

---

## v0.3.0 – Power Manager, CLI Dashboard, and CI

### Added

- **Power Manager Module**
  - New `power/` folder with `power_manager.c/.h`
  - Abstract power modes: `ACTIVE`, `IDLE`, `SLEEP`, `STOP`
  - Tracks current and requested modes
  - Logs mode transitions and counts idle cycles

- **CLI (Command Line Interface)**
  - New `common/cli.c/.h`
  - Line-based UART interface over USART2
  - Commands:
    - `help`
    - `log off|error|warn|info|debug|pause|resume`
    - `pmode active|idle|sleep|stop`
    - `status`
  - Pausable logging:
    - `log pause` temporarily disables task logs
    - `log resume` restores previous log state
  - Dashboard-style UX:
    - Logs scroll above
    - CLI prompt and partial input line are redrawn after each log line

- **Logging Enhancements**
  - Runtime log control:
    - `Log_Enable()/Log_IsEnabled()`
    - `Log_SetLevel()/Log_GetLevel()`
  - Log output now:
    - Starts with `\r` to avoid mixing with CLI prompt
    - Calls `CLI_OnExternalOutput()` to maintain a clean CLI line

- **GitHub Actions CI**
  - New workflow: `.github/workflows/ci.yml`
  - Uses `gcc-arm-none-eabi` on Ubuntu
  - Compile-only build of all `.c` sources on push/PR to `main`

---

## v0.2.0 – Sensor Abstraction and Simulated Sensor

### Added

- **Sensor Abstraction Layer**
  - `sensors/sensor_if.h` with `SensorIF_t` abstraction
  - Simulated temperature sensor implementation:
    - `sensors/sensor_sim_temp.c/.h`
  - Central function `Sensor_GetInterface()` to obtain the active interface

- **SensorSample Task**
  - Periodically reads data from the sensor interface
  - Logs value and timestamp via the logging subsystem

### Changed

- Application code now interacts only with the `SensorIF_t` interface, not with a specific sensor implementation.
- Prepared the design so that a real sensor (I2C, SPI, etc.) can replace the simulated one without touching `app_main`.

---

## v0.1.0 – Initial Skeleton and Logging

### Added

- STM32CubeIDE project targeting Nucleo-F446RE
- Task Manager:
  - Basic cooperative scheduler in `app/app_task_manager.c`
  - `Heartbeat` task that toggles the on-board LED
- Logging subsystem:
  - UART-based log output with timestamp and severity
  - Macros: `LOG_DEBUG`, `LOG_INFO`, `LOG_WARN`, `LOG_ERROR`
- Documentation:
  - Initial `docs/README.md` and `docs/ARCHITECTURE.md`
