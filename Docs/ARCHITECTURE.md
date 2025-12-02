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
  - Logging
  - CLI
  - Sensors
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
- `SensorSample` — reads simulated sensor data
- `PowerManager` — manages power modes
- `CLI` — processes UART command input

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
[00123456 ms][INF][../app/app_main.c:152][App_Task] Message...
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

### Thread safety glue (`newlib_lock_glue.c`)

Makes newlib thread-safe for interrupts/RTOS usage in future.

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
- Simulated temperature sensor (`sensor_if.c`)
- Clean pathway to drop in real I2C/SPI sensors later.

---

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
- Requested mode
- Idle cycle counts
- Mode transitions (logged via logging subsystem)

Future phases will integrate real STM32 low-power entry logic.

---

## 6. CLI Dashboard interaction

The logger and CLI share the same UART.
To maintain readability:

- Logs begin with `
` to reset cursor to column 0
- After printing logs, `Log_Print()` calls `CLI_OnExternalOutput()`
- CLI reprints:

```
> <current_input>
```

This creates a pseudo-dashboard effect.

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

Enforces professional software development discipline.
