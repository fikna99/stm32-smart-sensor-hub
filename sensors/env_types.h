/**
 * @file env_types.h
 * @brief Shared types for environment sensors (temperature, pressure, humidity).
 */

#ifndef ENV_TYPES_H
#define ENV_TYPES_H

#include <stdint.h>

/**
 * @brief Environment sample (temperature/pressure/humidity).
 */
typedef struct
{
    float    temperatureC;  /**< Temperature in degrees Celsius. */
    float    pressurePa;    /**< Pressure in Pascals. */
    float    humidityRH;    /**< Relative humidity in percent (0..100). */
    uint32_t timestampMs;   /**< Timestamp in ms (Log_GetTimestampMs). */
} EnvData_t;

#endif /* ENV_TYPES_H */
