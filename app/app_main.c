/**
 * @file app_main.c
 * @brief Application main logic for Smart Sensor Hub.
 */

#include "app_main.h"
#include "app_task_manager.h"
#include "log.h"
#include "stm32f4xx_hal.h"
#include "sensor_if.h"
#include "power_manager.h"   /**< New include for Phase 3. */
#include "cli.h"

/* Forward declaration of the heartbeat task function. */
static void App_TaskHeartbeat(void);

/**
 * @brief Periodic task that samples the active sensor and logs the result.
 *
 * This task retrieves the currently selected sensor interface (simulated in
 * Phase 2), performs a measurement, and logs the structured data including
 * timestamp and sensor value. It runs at a fixed interval defined by its
 * task descriptor.
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

static void App_TaskCli(void);

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
 * This task samples the sensor once per second and logs the measured value.
 * It acts as the foundation for future real hardware sensors (I2C/SPI) and
 * edge-processing features such as filtering or anomaly detection.
 */
static AppTaskDescriptor_t s_sensorTask =
{
    .name       = "SensorSample",  /**< Human-readable task name. */
    .function   = App_TaskSensorSample, /**< Task entry function. */
    .period_ms  = 1000U,           /**< Execute once every 1000 ms. */
    .lastRun_ms = 0U               /**< Populated at task registration. */
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

static AppTaskDescriptor_t s_cliTask =
{
    .name       = "CLI",
    .function   = App_TaskCli,
    .period_ms  = 20U,
    .lastRun_ms = 0U
};

void App_MainInit(void)
{
    LOG_INFO("Application initialization started");

    /* Initialize task manager and register tasks. */
    AppTaskManager_Init();

    /* Initialize power manager */
    PowerManager_Init();

    /* Initialize the active sensor interface. */
    const SensorIF_t *sensorIF = Sensor_GetInterface();
    if (!sensorIF->init())
    {
        LOG_ERROR("Sensor initialization failed");
    }

    /* Register periodic tasks */
    (void)AppTaskManager_RegisterTask(&s_heartbeatTask);
    (void)AppTaskManager_RegisterTask(&s_sensorTask);
    (void)AppTaskManager_RegisterTask(&s_powerTask);
    (void)AppTaskManager_RegisterTask(&s_cliTask);

    LOG_INFO("Application initialization completed");
}

void App_MainLoop(void)
{
    /* Run scheduler once to process any ready tasks. */
    AppTaskManager_RunOnce();
}

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
 * This task uses the currently active sensor interface (simulated for Phase 2)
 * to retrieve a measurement. Upon success, the function logs the result using
 * the global logging subsystem. Each sample includes the measured value and
 * the timestamp (in milliseconds) when the reading was taken.
 *
 * This task forms the basis for integrating real I2C/SPI sensors in later
 * phases without requiring changes to the task implementation itself.
 */
static void App_TaskSensorSample(void)
{
    /* Retrieve active sensor interface */
    const SensorIF_t *sensorIF = Sensor_GetInterface();
    SensorData_t data;

    /* Attempt to read sensor data */
    if (sensorIF->read(&data))
    {
        LOG_INFO("SensorSample: value=%.2f C, timestamp=%lu ms",
                 data.value,
                 (unsigned long)data.timestamp);
    }
    else
    {
        LOG_WARN("SensorSample: sensor read failed");
    }
}

/**
 * @brief Periodically service the power manager.
 *
 * This function is invoked by the Task Manager at a fixed period.
 * In Phase 3 it simply delegates to PowerManager_Update(), which
 * logs state transitions and maintains an idle cycle count.
 *
 * Future phases may extend this to adjust sensor sampling rates,
 * trigger low-power entry, or expose power statistics via a CLI.
 */
static void App_TaskPowerManager(void)
{
    PowerManager_Update();
}

/**
 * @brief Periodic wrapper around CLI processing.
 */
static void App_TaskCli(void)
{
    CLI_Process();
}
