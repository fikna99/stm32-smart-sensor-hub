/**
 * @file sensor_sim_temp.h
 * @brief Interface for the simulated temperature sensor backend.
 *
 * The implementation of this interface is provided in sensor_sim_temp.c.
 * The application does not call this directly; instead, sensor_if.c
 * selects this backend when APP_USE_SIMULATED_SENSOR is enabled.
 */

#ifndef SENSOR_SIM_TEMP_H
#define SENSOR_SIM_TEMP_H

#include "sensor_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the SensorIF_t for the simulated temperature sensor.
 *
 * @return Pointer to a constant SensorIF_t describing the simulated sensor.
 */
const SensorIF_t * SensorSimTemp_GetInterface(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_SIM_TEMP_H */
