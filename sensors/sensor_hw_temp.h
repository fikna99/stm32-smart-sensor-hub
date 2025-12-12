/**
 * @file sensor_hw_temp.h
 * @brief Hardware-backed temperature sensor using I2C.
 *
 * This module provides a concrete SensorIF_t implementation for an
 * I2C temperature sensor (LM75/TMP102-style) connected to I2C1.
 */

#ifndef SENSOR_HW_TEMP_H
#define SENSOR_HW_TEMP_H

#include <stdbool.h>
#include "sensor_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the SensorIF_t for the hardware I2C temperature sensor.
 *
 * @return Pointer to a constant SensorIF_t describing the hardware sensor.
 */
const SensorIF_t * SensorHWTemp_GetInterface(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_HW_TEMP_H */
