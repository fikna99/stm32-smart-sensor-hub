/**
 * @file sensor_sim_temp.c
 * @brief Simulated temperature sensor implementation.
 *
 * This module provides a synthetic temperature sensor used in early phases
 * of the Smart Sensor Hub project. It generates a smooth, time-varying
 * signal based on a sine wave to emulate realistic sensor behavior without
 * requiring actual hardware.
 */

#include "sensor_sim_temp.h"
#include "stm32f4xx_hal.h"
#include <math.h>

/**
 * @brief Internal state storing the simulation start time.
 */
static uint32_t s_simStartTime_ms = 0U;

/* Forward declarations of implementation functions. */
static bool SensorSimTemp_InitImpl(void);
static bool SensorSimTemp_ReadImpl(SensorData_t *outData);

/**
 * @brief Static instance of the simulated sensor interface.
 *
 * This structure is returned to the application via
 * SensorSimTemp_GetInterface() so that sensor_if.c can expose it
 * through the generic SensorIF_t API.
 */
static const SensorIF_t s_simTempIF =
{
    .init = SensorSimTemp_InitImpl,
    .read = SensorSimTemp_ReadImpl
};

/* ------------------------------------------------------------------------- */
/*                      Public Interface                                     */
/* ------------------------------------------------------------------------- */

const SensorIF_t * SensorSimTemp_GetInterface(void)
{
    return &s_simTempIF;
}

/* ------------------------------------------------------------------------- */
/*                      Static Implementation Functions                      */
/* ------------------------------------------------------------------------- */

/**
 * @brief Initialize the simulated temperature sensor.
 *
 * This function records the current system tick as the reference time
 * for the simulation. All subsequent readings are based on the time
 * elapsed since this point.
 *
 * @retval true  Initialization completed successfully.
 * @retval false Initialization failed (not expected for this backend).
 */
static bool SensorSimTemp_InitImpl(void)
{
    s_simStartTime_ms = HAL_GetTick();
    return true;
}

/**
 * @brief Generate a simulated temperature reading.
 *
 * The simulated temperature follows a simple sine wave over time:
 *
 *   T(t) = 25.0°C + 3.0°C * sin( t / 2000 ms )
 *
 * @param[out] outData Pointer to a SensorData_t structure that will be
 *                     populated with the simulated measurement.
 *
 * @retval true  Reading generated successfully.
 * @retval false Pointer @p outData was NULL.
 */
static bool SensorSimTemp_ReadImpl(SensorData_t *outData)
{
    if (outData == NULL)
    {
        return false;
    }

    uint32_t now_ms     = HAL_GetTick();
    uint32_t elapsed_ms = now_ms - s_simStartTime_ms;

    float phase = (float)elapsed_ms / 2000.0f;

    /* Base temperature = 25°C, amplitude = ±3°C. */
    float tempC = 25.0f + 3.0f * sinf(phase);

    outData->value     = tempC;
    outData->timestamp = now_ms;
    outData->mode      = 0U; /* Unused in the simulated backend. */

    return true;
}
