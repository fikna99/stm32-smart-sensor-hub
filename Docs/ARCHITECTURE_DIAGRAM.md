# System Architecture Diagram

This diagram mirrors the "box-and-arrow" style used in typical system architecture docs.
It focuses on the firmware modules and how they interact at runtime.

```mermaid
flowchart LR
    subgraph MCU[STM32 Smart Sensor Hub Firmware (STM32F446RE)]
        subgraph HAL[Hardware + HAL]
            CORE[Core/ Startup + Clock + ISR]
            PERIPH[UART / I²C / SPI / GPIO]
        end

        subgraph APP[Application Layer]
            MAIN[App_MainInit / App_MainLoop]
            TM[Task Manager (cooperative)]
            TASKS[Tasks: Heartbeat, EnvSample, LightSample, PowerManager, CLI]
        end

        subgraph COMMON[Common]
            LOG[Logging]
            CLI[CLI (UART)]
        end

        subgraph SENS[Sensors]
            IF[Sensor Interfaces]
            BME[BME280 Env (SPI2)]
            TSL[TSL2591 Light (I²C)]
            SIM[Sim backends (optional)]
        end

        subgraph PWR[Power]
            PM[Power Manager]
            MODES[Modes: ACTIVE/IDLE/SLEEP/STOP]
        end
    end

    CORE --> MAIN
    PERIPH --> COMMON
    MAIN --> TM
    TM --> TASKS
    TASKS --> LOG
    TASKS --> CLI
    TASKS --> PM
    PM --> MODES
    TASKS --> IF
    IF --> BME
    IF --> TSL
    IF --> SIM
    CLI --> PERIPH
    LOG --> PERIPH
```
