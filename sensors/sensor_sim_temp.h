/**
 * @file sensor_sim_temp.h
 * @brief Interface for the simulated temperature sensor.
 *
 * The implementation of this interface is provided in
 * sensor_sim_temp.c and is exposed to the application via
 * Sensor_GetInterface() declared in sensor_if.h.
 *
 * @ingroup sensors
 */

#ifndef SENSOR_SIM_TEMP_H
#define SENSOR_SIM_TEMP_H

#include "sensor_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/* No extra API for now; the application uses Sensor_GetInterface()
 * from sensor_if.h to access this implementation.
 */

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_SIM_TEMP_H */
