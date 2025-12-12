/**
 * @file light_tsl2591.h
 * @brief Driver for the TSL2591 ambient light sensor (I2C).
 *
 * This module provides a small blocking driver for the TSL2591.
 * It is written for STM32 HAL I2C and uses the shared LOG_*
 * macros for diagnostics.
 *
 * The driver exposes a simple "init + read" API so it can be
 * plugged into the existing task framework.
 */

#ifndef LIGHT_TSL2591_H
#define LIGHT_TSL2591_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TSL2591 integration time configuration.
 *
 * See datasheet Control register ATIME field (0x01).
 */
typedef enum
{
    TSL2591_IT_100MS = 0U,  /**< 100 ms integration time. */
    TSL2591_IT_200MS = 1U,  /**< 200 ms integration time. */
    TSL2591_IT_300MS = 2U,  /**< 300 ms integration time. */
    TSL2591_IT_400MS = 3U,  /**< 400 ms integration time. */
    TSL2591_IT_500MS = 4U,  /**< 500 ms integration time. */
    TSL2591_IT_600MS = 5U   /**< 600 ms integration time. */
} TSL2591_IntegrationTime_t;

/**
 * @brief TSL2591 gain configuration.
 *
 * See datasheet Control register AGAIN field (0x01).
 */
typedef enum
{
    TSL2591_GAIN_LOW    = 0U,  /**< Low gain.    */
    TSL2591_GAIN_MEDIUM = 1U,  /**< Medium gain. */
    TSL2591_GAIN_HIGH   = 2U,  /**< High gain.   */
    TSL2591_GAIN_MAX    = 3U   /**< Maximum gain */
} TSL2591_Gain_t;

/**
 * @brief Single ambient light sample (lux + raw counts).
 */
typedef struct
{
    float    lux;           /**< Calculated approximate lux.         */
    uint16_t fullChannel;   /**< CH0 (full spectrum) counts.         */
    uint16_t irChannel;     /**< CH1 (IR) counts.                    */
    uint32_t timestampMs;   /**< Timestamp in ms (Log_GetTimestampMs). */
} TSL2591_Data_t;

/**
 * @brief Initialize the TSL2591 driver and power up the sensor.
 *
 * This function:
 *  - stores the I2C handle internally,
 *  - verifies the device ID,
 *  - configures integration time and gain,
 *  - powers on ALS and starts continuous conversions.
 *
 * It is safe to call this once at startup from App_MainInit().
 *
 * @param[in] it   Integration time configuration.
 * @param[in] gain Gain configuration.
 *
 * @retval true  Sensor initialized successfully.
 * @retval false Initialization failed (I2C error or wrong device ID).
 */
bool TSL2591_Init(TSL2591_IntegrationTime_t it, TSL2591_Gain_t gain);

/**
 * @brief Read a single ambient light sample.
 *
 * This reads both ALS channels (CH0 and CH1), calculates an
 * approximate lux value, and timestamps the sample.
 *
 * This is a blocking call and assumes that the integration
 * time has already elapsed since the last enable. With the
 * default configuration (e.g. 200 ms), calling this at 1 Hz
 * is perfectly acceptable.
 *
 * @param[out] out Pointer to a TSL2591_Data_t structure to populate.
 *
 * @retval true  Sample read successfully.
 * @retval false I2C error.
 */
bool TSL2591_Read(TSL2591_Data_t *out);

#ifdef __cplusplus
}
#endif

#endif /* LIGHT_TSL2591_H */
