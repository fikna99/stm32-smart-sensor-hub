# Architecture Overview

The STM32 Smart Sensor Hub firmware is divided into modular layers to maintain clean separation of concerns, easy testability, and future expansion.

---

## 1. Hardware & HAL Layer

Located in `Core/` and `Drivers/STM32F4xx_HAL_Driver/`.

Responsibilities:
- System startup & reset vector
- Clock configuration
- GPIO initialization
- UART initialization (USART2)
- SysTick management
- HAL drivers for peripherals

This layer is generated & maintained by STM32CubeIDE, with hand edits where required.

---

## 2. Application Layer (`app/`)

Contains high-level program control:

### `app_main.c`

- `App_MainInit()`  
  Initializes all modules:
  - Logging subsystem
  - CLI
  - Sensors (via `SensorIF_t`)
  - Power Manager
  - Task Manager

- `App_MainLoop()`  
  Runs the cooperative scheduler:

```c
while (1)
{
    AppTaskManager_RunOnce();
}
```

### Task Manager (`app_task_manager.c`)

Implements a cooperative scheduler with periodic tasks.

Data structure:

```c
typedef struct {
    const char *name;
    void (*function)(void);
    uint32_t period_ms;
    uint32_t lastRun_ms;
} AppTaskDescriptor_t;
```

Registered Tasks:
- `Heartbeat` — toggles LED, system liveness
- `EnvSample` — reads BME280 (or simulated env) and logs T/P/H
- `LightSample` — reads TSL2591 and logs lux/ALS values
- `PowerManager` — manages power modes
- `CLI` — processes UART command input

The **SensorSample** task period is not hard-coded; it is derived from the
current power mode, using:

```c
#define SENSOR_PERIOD_ACTIVE_MS   (1000U)
#define SENSOR_PERIOD_IDLE_MS     (5000U)
#define SENSOR_PERIOD_SLEEP_MS    (30000U)
#define SENSOR_PERIOD_STOP_MS     (0U)   // 0 => no sampling in STOP
```

When `POWER_MODE_STOP` is active, the period is 0 and the sampling
task is effectively disabled (no sensor reads).

Each run produces `DEBUG` logs showing scheduling behavior.

---

## 3. Common Layer (`common/`)

### Logging subsystem (`log.c/.h`)

Features:
- Debug/info/warn/error levels
- Runtime logging control
- Pause/resume support
- Standardized format:

```
[00123456 ms][INF][../app/app_main.c:152][App_TaskSensorSample] Message...
```

- Cursor management so logs never corrupt CLI input
- Calls `CLI_OnExternalOutput()` so CLI redraws prompt cleanly

### CLI subsystem (`cli.c/.h`)

Features:
- Line-buffered UART command interface
- Supports:
  - Backspace handling
  - Command parsing
  - Clean dashboard-like UI
- Commands:
  - `log off|error|warn|info|debug|pause|resume`
  - `pmode active|idle|sleep|stop`
  - `status`
  - `help`

The `status` command reports the **effective sensor sampling period**
that is in use based on the current power mode.


---

## 4. Sensor Layer (`sensors/`)

Implements a fully abstracted sensor interface.

Interface:

```c
typedef struct {
    bool (*init)(void);
    bool (*read)(SensorData_t *out);
} SensorIF_t;
```

Provides:
- `sensor_if.h` with the generic `SensorIF_t` API
- Sensor backends:

- Simulated temperature backend (`sensor_sim_temp.c/.h`) for early bring-up
- TSL2591 light sensor backend (I²C)
- BME280 environmental sensor backend (SPI2)

The application consumes sensors through interfaces so backends can be swapped without changing `app/` code.

This design makes it trivial to drop in real I²C/SPI/ADC sensors later
without changing application code.

```c
typedef enum {
    POWER_MODE_ACTIVE,
    POWER_MODE_IDLE,
    POWER_MODE_SLEEP,
    POWER_MODE_STOP
} PowerMode_t;
```

## 5. Power Management Layer (`power/`)

Manages abstract power modes:

```c
typedef enum {
    POWER_MODE_ACTIVE,
    POWER_MODE_IDLE,
    POWER_MODE_SLEEP,
    POWER_MODE_STOP
} PowerMode_t;
```

Tracks:
- Current power mode
- Requested power mode
- Idle cycle counts
- Mode transitions (logged via logging subsystem)

The power mode indirectly drives the **SensorSample** task behavior by
selecting one of the configured sampling periods. In STOP mode, sampling
is fully disabled (period 0).

Future phases will integrate real STM32 low-power entry logic (sleep/stop)
and wakeup sources.

---

## 6. CLI Dashboard Interaction

The logger and CLI share the same UART.  
To maintain readability:

- Logs begin with `\r` to reset cursor to column 0
- After printing logs, `Log_Print()` calls `CLI_OnExternalOutput()`
- CLI reprints:

```text
> <current_input>
```

This creates a pseudo-dashboard effect where logs scroll *above* and the
command line remains anchored at the bottom.

---

## 7. GitHub Actions CI

Workflow located at:

```
.github/workflows/ci.yml
```

Performs:
- Install ARM GCC
- Compile-only firmware build
- Ensures clean code on every push/PR

Enforces professional software development discipline for the project.
