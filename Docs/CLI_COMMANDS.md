# CLI Commands

The Smart Sensor Hub exposes a UART-based command-line interface (CLI) over the ST-LINK VCP (USART2).  
Default settings: **115200 baud, 8 data bits, no parity, 1 stop bit (115200 8N1)**.

On reset you should see:

```text
Smart Sensor Hub CLI ready.
Type 'help' for a list of commands.
>
```

You can then type commands followed by Enter.

---

## General Behavior

- Input is **line-based**: the command is processed when you press Enter.
- Backspace is supported.
- Unknown commands generate a clear error message.
- The CLI prompt is always kept at the bottom like a dashboard:
  - Log messages scroll above.
  - Prompt is redrawn after each log line.

---

## Tip: Reading CLI output cleanly

If task logs are scrolling while you are trying to read `help`, use:

```text
> log pause
```

Then run your command (e.g., `help`), and restore logging with:

```text
> log resume
```

---

## Command Reference

### `help`

**Usage:**

```text
> help
```

**Description:**  
Lists all available commands with a short description.

---

### `log off`

Disables all **task logging** (logs from `LOG_*` macros).  
CLI output remains active.

```text
> log off
Task logging disabled.
```

---

### `log error`

Enables task logging with **ERROR** level only.

```text
> log error
Task logging enabled, level=ERROR.
```

---

### `log warn`

Enables task logging with **WARN and ERROR** messages.

```text
> log warn
Task logging enabled, level=WARN.
```

---

### `log info`

Enables task logging with **INFO, WARN, and ERROR** messages.

```text
> log info
Task logging enabled, level=INFO.
```

---

### `log debug`

Enables task logging with **DEBUG, INFO, WARN, and ERROR** messages.

```text
> log debug
Task logging enabled, level=DEBUG.
```

This is the most verbose setting and is very useful while developing.

---

### `log pause`

Temporarily **pauses task logging** while remembering the previous log level and enabled state.

```text
> log pause
Task logging paused. Use 'log resume' to restore.
```

Use this when you want to type or read CLI output without logs scrolling.

---

### `log resume`

Restores the logging configuration saved by `log pause`.

```text
> log resume
Task logging resumed.
```

If logging was not paused, you see:

```text
Task logging is not paused.
```

---

### `pmode <mode>`

Requests a change in the system **power mode**.

Supported modes:

- `active`
- `idle`
- `sleep`
- `stop`

**Example:**

```text
> pmode idle
Requested power mode change: idle
```

Currently (v0.5.0) the Power Manager:

- Logs mode transitions.
- Tracks idle cycles.
- Drives the **sensor sampling period** used by the `SensorSample` task:
  - ACTIVE  → 1000 ms
  - IDLE    → 5000 ms
  - SLEEP   → 30000 ms
  - STOP    → 0 ms (no sampling)

Future phases may map these modes to real STM32 low-power states.

---

### `status`

Displays the current logging configuration, power mode, and effective sensor sample period.

```text
> status

Status:
  Task logging: ENABLED
  LogLevel: 1 (0=DEBUG,1=INFO,2=WARN,3=ERROR)
  PowerMode: 2 (0=ACTIVE,1=IDLE,2=SLEEP,3=STOP)
  Sensor sample period: 30000 ms
```

Where:

- **Task logging** → whether task logs are enabled
- **LogLevel** → current minimum log level
- **PowerMode** → current abstract mode
- **Sensor sample period** → effective period based on power mode  
  (0 in STOP mode, meaning sampling is disabled)

---

## Example Session

```text
Smart Sensor Hub CLI ready.
Type 'help' for a list of commands.

> help
Available commands:
  help            - Show this help text
  log off         - Disable all task logging
  log error       - Task logs at ERROR level only
  log warn        - Task logs at WARN and above
  log info        - Task logs at INFO and above
  log debug       - Task logs at DEBUG and above
  log pause       - Temporarily pause all task logs
  log resume      - Resume task logs to previous state
  pmode active    - Request POWER_MODE_ACTIVE
  pmode idle      - Request POWER_MODE_IDLE
  pmode sleep     - Request POWER_MODE_SLEEP
  pmode stop      - Request POWER_MODE_STOP
  status          - Show logging and power status

> log debug
Task logging enabled, level=DEBUG.

[00001234 ms][DBG][../app/app_task_manager.c:77][AppTaskManager_RunOnce] Running task 'Heartbeat' (elapsed: 500 ms)

> pmode sleep
Requested power mode change: sleep

> status
Status:
  Task logging: ENABLED
  LogLevel: 0 (0=DEBUG,1=INFO,2=WARN,3=ERROR)
  PowerMode: 2 (0=ACTIVE,1=IDLE,2=SLEEP,3=STOP)
  Sensor sample period: 30000 ms

> pmode stop
Requested power mode change: stop

> status
Status:
  Task logging: ENABLED
  LogLevel: 0 (0=DEBUG,1=INFO,2=WARN,3=ERROR)
  PowerMode: 3 (0=ACTIVE,1=IDLE,2=SLEEP,3=STOP)
  Sensor sample period: 0 ms (sampling disabled in STOP)
```

This file serves as a quick reference for anyone interacting with the firmware via UART.
