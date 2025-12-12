/**
 * @file sensor_if.h
 * @brief Public sensor interface types and API.
 *
 * This header defines the generic sensor interface used by the application.
 * The actual backend (simulated vs hardware) is selected in sensor_if.c.
 */

#ifndef SENSOR_IF_H
#define SENSOR_IF_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generic sensor data structure returned by sensor backends.
 */
typedef struct
{
    float    value;      /**< Measured value (e.g., temperature in °C).   */
    uint32_t timestamp;  /**< Timestamp in milliseconds.                  */
    uint32_t mode;       /**< Optional: power mode or additional context. */
} SensorData_t;

/**
 * @brief Interface for a generic sensor.
 *
 * All sensor backends (simulated, hardware, etc.) implement this interface.
 */
typedef struct
{
    bool (*init)(void);             /**< Initialize the sensor backend.       */
    bool (*read)(SensorData_t *out);/**< Read current sensor value.           */
} SensorIF_t;

/**
 * @brief Initialize and select the active sensor interface.
 *
 * This must be called once from App_MainInit() before Sensor_GetInterface().
 * The backend (simulated vs hardware) is selected at compile time using
 * APP_USE_SIMULATED_SENSOR / APP_USE_HW_SENSOR in app_config.h.
 *
 * @retval true  Sensor backend initialized successfully.
 * @retval false Initialization failed.
 */
bool SensorIF_Init(void);

/**
 * @brief Get the active SensorIF_t pointer.
 *
 * @return Pointer to SensorIF_t, or NULL if SensorIF_Init() failed.
 */
const SensorIF_t * Sensor_GetInterface(void);

#ifdef __cplusplus
}
#endif

/** @} */ /* end of sensors group */

#endif /* SENSOR_IF_H */
