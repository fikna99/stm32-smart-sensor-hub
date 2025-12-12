/**
 * @file app_main.c
 * @brief Application main logic for Smart Sensor Hub.
 * @ingroup app
 *
 * This module wires together the core subsystems of the Smart Sensor Hub:
 * - Cooperative scheduler (task manager)
 * - Logging subsystem
 * - Power manager
 * - Sensor abstraction layer
 * - CLI interface
 *
 * It defines and registers periodic tasks that demonstrate a power-aware,
 * sensor-driven application on STM32.
 */

#include "app_main.h"
#include "app_task_manager.h"
#include "log.h"
#include "stm32f4xx_hal.h"
#include "sensor_if.h"
#include "power_manager.h"
#include "cli.h"
#include "app_config.h"
#include "light_if.h"
#include "light_tsl2591.h"    /* For TSL2591_Data_t */
#include "env_if.h"
#include "env_types.h"



/* ------------------------------------------------------------------------- */
/* Forward declarations                                                      */
/* ------------------------------------------------------------------------- */

/**
 * @brief Periodic heartbeat task that toggles the LED and logs a message.
 */
static void App_TaskHeartbeat(void);

/**
 * @brief Periodic task that samples the active sensor and logs the result.
 *
 * This task retrieves the currently selected sensor interface (simulated in
 * Phase 2), performs a measurement, and logs the structured data including
 * timestamp and sensor value. It runs at an effective interval determined
 * by both its scheduler period and the current power mode.
 */
static void App_TaskSensorSample(void);

/**
 * @brief Periodic task that updates the power manager.
 *
 * This task calls PowerManager_Update() at a fixed interval so that
 * power mode transitions and related policies can be evaluated and
 * logged centrally.
 */
static void App_TaskPowerManager(void);

/**
 * @brief Periodic wrapper around CLI processing.
 *
 * This task periodically invokes CLI_Process() to handle incoming UART
 * characters and dispatch CLI commands. It is scheduled at a relatively
 * high rate to keep the UI responsive.
 */
static void App_TaskCli(void);

/**
 * @brief Periodic task: sample ambient light from TSL2591.
 */
static void App_TaskLightSample(void);

static void App_TaskEnvSample(void);
/* ------------------------------------------------------------------------- */
/* Task descriptors                                                          */
/* ------------------------------------------------------------------------- */

/**
 * @brief Descriptor for the heartbeat task.
 *
 * This task toggles the on-board LED and logs a message at
 * a fixed period, demonstrating that the scheduler and logging
 * are functioning correctly.
 */
static AppTaskDescriptor_t s_heartbeatTask =
{
    .name       = "Heartbeat",
    .function   = App_TaskHeartbeat,
    .period_ms  = 500U,
    .lastRun_ms = 0U
};

/**
 * @brief Task descriptor for the Sensor Sampling task.
 *
 * This task periodically considers sampling the sensor, but the actual
 * sampling interval is power-aware and controlled by App_TaskSensorSample()
 * using the periods defined in @ref app_config.
 */
static AppTaskDescriptor_t s_sensorTask =
{
    .name       = "SensorSample",        /**< Human-readable task name.         */
    .function   = App_TaskSensorSample,  /**< Task entry function.              */
    .period_ms  = 1000U,                 /**< Scheduler tick for this task.     */
    .lastRun_ms = 0U                     /**< Populated at task registration.   */
};

/**
 * @brief Task descriptor for the Power Manager task.
 *
 * This task runs at a moderate frequency (e.g. every 500 ms) and
 * evaluates the current power mode and any outstanding requests.
 */
static AppTaskDescriptor_t s_powerTask =
{
    .name       = "PowerManager",        /**< Human-readable task name. */
    .function   = App_TaskPowerManager,  /**< Task entry function.      */
    .period_ms  = 500U,                  /**< Execute every 500 ms.     */
    .lastRun_ms = 0U
};

/**
 * @brief Task descriptor for the CLI processing task.
 */
static AppTaskDescriptor_t s_cliTask =
{
    .name       = "CLI",
    .function   = App_TaskCli,
    .period_ms  = 20U,
    .lastRun_ms = 0U
};

static AppTaskDescriptor_t s_lightTask =
{
	    .name       = "LightSample",        /**< Human-readable task name.         */
	    .function   = App_TaskLightSample,  /**< Task entry function.              */
	    .period_ms  = 1000U,                 /**< Scheduler tick for this task.     */
	    .lastRun_ms = 0U                     /**< Populated at task registration.   */
};

static AppTaskDescriptor_t s_envTask =
{
	    .name       = "EnvSample",        /**< Human-readable task name.         */
	    .function   = App_TaskEnvSample,  /**< Task entry function.              */
	    .period_ms  = 2000U,                 /**< Scheduler tick for this task.     */
	    .lastRun_ms = 0U                     /**< Populated at task registration.   */
};
/* ------------------------------------------------------------------------- */
/* Public API                                                                */
/* ------------------------------------------------------------------------- */

void App_MainInit(void)
{
    LOG_INFO("Application initialization started");

    /* Initialize task manager and register tasks. */
    AppTaskManager_Init();

    /* Initialize power manager. */
    PowerManager_Init();

    /* Initialize the active sensor interface (selects SIM or HW backend). */
    if (!SensorIF_Init())
    {
        LOG_ERROR("App_MainInit: SensorIF_Init() failed");
    }
    else
    {
        LOG_INFO("App_MainInit: sensor interface initialized successfully");
    }

    if (!LightIF_Init())
    {
        LOG_ERROR("App_MainInit: LightIF_Init() failed");
    }
    else
    {
        LOG_INFO("App_MainInit: light interface initialized successfully");
    }

    if (!EnvIF_Init())
    {
        LOG_ERROR("App_MainInit: EnvIF_Init failed.");
    }
    else
    {
        LOG_INFO("App_MainInit: EnvIF initialized.");
    }


    /* Register periodic tasks with the scheduler. */
    (void)AppTaskManager_RegisterTask(&s_heartbeatTask);
    (void)AppTaskManager_RegisterTask(&s_sensorTask);
    (void)AppTaskManager_RegisterTask(&s_powerTask);
    (void)AppTaskManager_RegisterTask(&s_cliTask);
    (void)AppTaskManager_RegisterTask(&s_lightTask);
    (void)AppTaskManager_RegisterTask(&s_envTask);

    LOG_INFO("Application initialization completed");
}

void App_MainLoop(void)
{
    /* Run scheduler once to process any ready tasks. */
    AppTaskManager_RunOnce();
}

/* ------------------------------------------------------------------------- */
/* Task implementations                                                      */
/* ------------------------------------------------------------------------- */

static void App_TaskHeartbeat(void)
{
    /* This assumes the on-board LED is connected to GPIOA Pin 5 (Nucleo-64). */
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    LOG_INFO("Heartbeat task toggled LED");
}

/**
 * @brief Executes one sensor sampling cycle.
 *
 * This task uses the currently active sensor interface (simulated or
 * hardware-backed, depending on configuration) to retrieve a measurement.
 *
 * The effective sampling rate is power-aware and is derived from the
 * SENSOR_PERIOD_* constants defined in @ref app_config.
 */
static void App_TaskSensorSample(void)
{
    static uint32_t lastSampleTick_ms = 0U;

    const SensorIF_t *sensorIF = Sensor_GetInterface();
    if (sensorIF == NULL)
    {
        LOG_ERROR("SensorSample: Sensor interface is NULL");
        return;
    }

    /* Determine desired sampling period based on power mode. */
    PowerMode_t mode = PowerManager_GetCurrentMode();
    uint32_t now_ms  = HAL_GetTick();
    uint32_t period_ms = 0U;

    switch (mode)
    {
        case POWER_MODE_ACTIVE:
            period_ms = SENSOR_PERIOD_ACTIVE_MS;
            break;

        case POWER_MODE_IDLE:
            period_ms = SENSOR_PERIOD_IDLE_MS;
            break;

        case POWER_MODE_SLEEP:
            period_ms = SENSOR_PERIOD_SLEEP_MS;
            break;

        case POWER_MODE_STOP:
        default:
            period_ms = SENSOR_PERIOD_STOP_MS;
            break;
    }

    /* If period is 0, sampling is disabled in this mode. */
    if (period_ms == 0U)
    {
        LOG_DEBUG("SensorSample: sampling disabled in current power mode (%d)", (int)mode);
        return;
    }

    /* Check if it's time to sample again. */
    if ((now_ms - lastSampleTick_ms) < period_ms)
    {
        /* Not yet time to sample; exit early. */
        return;
    }

    lastSampleTick_ms = now_ms;

    /* Perform the actual sensor read. */
    SensorData_t data;
    if (sensorIF->read(&data))
    {
        LOG_INFO("SensorSample: value=%.2f C, timestamp=%lu ms, mode=%d",
                 data.value,
                 (unsigned long)data.timestamp,
                 (int)mode);
    }
    else
    {
        LOG_WARN("SensorSample: read failed (mode=%d)", (int)mode);
    }
}

/**
 * @brief Periodically service the power manager.
 *
 * This function is invoked by the Task Manager at a fixed period.
 * In Phase 3/4 it simply delegates to PowerManager_Update(), which
 * logs state transitions and maintains an idle cycle count.
 *
 * Future phases may extend this to adjust sensor sampling rates,
 * trigger low-power entry, or expose power statistics via the CLI.
 */
static void App_TaskPowerManager(void)
{
    PowerManager_Update();
}

/**
 * @brief Periodic wrapper around CLI processing.
 *
 * This task keeps the UART-based CLI responsive by polling CLI_Process()
 * at a fixed rate. The CLI in turn handles user commands such as
 * power mode changes, logging controls, and status queries.
 */
static void App_TaskCli(void)
{
    CLI_Process();
}

/**
 * @brief Periodic task: sample ambient light from active backend.
 */
static void App_TaskLightSample(void)
{
    TSL2591_Data_t data;

    if (LightIF_Read(&data))
    {
        LOG_INFO("LightSample: lux=%.2f, full=%u, ir=%u, t=%lu ms",
                 (double)data.lux,
                 (unsigned int)data.fullChannel,
                 (unsigned int)data.irChannel,
                 (unsigned long)data.timestampMs);
    }
    else
    {
        LOG_WARN("LightSample: read failed.");
    }
}

static void App_TaskEnvSample(void)
{
    EnvData_t env;

    if (EnvIF_Read(&env))
    {
        LOG_INFO("EnvSample: T=%.2f C, P=%.2f Pa, H=%.2f %%RH, t=%lu ms",
                 (double)env.temperatureC,
                 (double)env.pressurePa,
                 (double)env.humidityRH,
                 (unsigned long)env.timestampMs);
    }
    else
    {
        LOG_WARN("EnvSample: read failed.");
    }
}

