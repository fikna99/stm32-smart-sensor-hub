/**
 * @file sensor_if.c
 * @brief Sensor interface selector (simulated vs hardware).
 *
 * This module returns the active SensorIF_t used by the application.
 * The backend is chosen at compile time via APP_USE_SIMULATED_SENSOR /
 * APP_USE_HW_SENSOR in app_config.h.
 */

#include "sensor_if.h"
#include "app_config.h"
#include "log.h"

#include "sensor_sim_temp.h"
#include "sensor_hw_temp.h"

static const SensorIF_t * s_activeIF = NULL;

/**
 * @brief Initialize and select the active sensor interface.
 *
 * This is called once from App_MainInit(). It:
 *  - chooses the backend (simulated or hardware)
 *  - calls its .init() function
 *
 * @retval true  Sensor backend initialized successfully.
 * @retval false Initialization failed.
 */
bool SensorIF_Init(void)
{
#if (APP_USE_SIMULATED_SENSOR == 1U)
    s_activeIF = SensorSimTemp_GetInterface();
    LOG_INFO("SensorIF_Init: using SIMULATED sensor backend.");
#elif (APP_USE_HW_SENSOR == 1U)
    s_activeIF = SensorHWTemp_GetInterface();
    LOG_INFO("SensorIF_Init: using HARDWARE I2C sensor backend.");
#else
#error "Exactly one of APP_USE_SIMULATED_SENSOR or APP_USE_HW_SENSOR must be 1."
#endif

    if ((s_activeIF == NULL) || (s_activeIF->init == NULL))
    {
        LOG_ERROR("SensorIF_Init: active interface is invalid.");
        return false;
    }

    if (!s_activeIF->init())
    {
        LOG_ERROR("SensorIF_Init: backend init failed.");
        return false;
    }

    return true;
}

/**
 * @brief Get the active SensorIF_t pointer.
 *
 * @return Pointer to SensorIF_t, or NULL if SensorIF_Init() failed.
 */
const SensorIF_t * Sensor_GetInterface(void)
{
    return s_activeIF;
}
