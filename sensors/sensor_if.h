/**
 * @file sensor_if.h
 * @brief Generic sensor interface for the Smart Sensor Hub project.
 *
 * This header defines a common C API for sensors (simulated or real).
 * Application code interacts only with this interface, regardless of
 * whether the underlying implementation talks to real hardware or
 * generates simulated data.
 *
 * @ingroup sensors
 */

#ifndef SENSOR_IF_H
#define SENSOR_IF_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup sensors Sensor Abstraction
 * @brief Generic sensor API and implementations for the Smart Sensor Hub.
 * @ingroup app
 * @{
 */

/**
 * @brief Structured sensor measurement data.
 *
 * For Phase 2, this represents a single scalar value (e.g. temperature).
 * Future phases may extend this with additional fields such as status
 * flags, error codes, or multi-dimensional measurements.
 */
typedef struct
{
    float    value;     /**< Sensor reading (e.g. temperature in °C). */
    uint32_t timestamp; /**< Timestamp in milliseconds (HAL_GetTick). */
} SensorData_t;

/**
 * @brief Function pointer type for sensor initialization.
 *
 * @return true if initialization succeeded, false otherwise.
 */
typedef bool (*SensorInitFn_t)(void);

/**
 * @brief Function pointer type for reading a sensor measurement.
 *
 * @param[out] outData Pointer to a SensorData_t structure that will be
 *                     filled with the measurement result on success.
 * @return true if the read operation succeeded, false otherwise.
 */
typedef bool (*SensorReadFn_t)(SensorData_t *outData);

/**
 * @brief Sensor interface API (function pointers).
 *
 * Any specific sensor implementation (simulated or hardware-backed)
 * must provide an instance of this structure so that the application
 * can invoke the sensor in a generic way.
 */
typedef struct
{
    SensorInitFn_t init; /**< Initialize the sensor module. */
    SensorReadFn_t read; /**< Read a single measurement.    */
} SensorIF_t;

/**
 * @brief Obtain the currently active sensor interface.
 *
 * In Phase 2, this returns a handle to the simulated temperature
 * sensor implementation. Later phases may route this to real I2C/SPI
 * sensor drivers or select between multiple sensors at runtime.
 *
 * @return Pointer to a constant SensorIF_t interface instance.
 */
const SensorIF_t *Sensor_GetInterface(void);

/** @} */ /* end of sensors group */

#endif /* SENSOR_IF_H */
