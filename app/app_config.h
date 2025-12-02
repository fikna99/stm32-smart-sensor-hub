/**
 * @file app_config.h
 * @brief Application-wide configuration constants for the Smart Sensor Hub.
 * @ingroup app
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include <stdint.h>

/**
 * @defgroup app_config Application Configuration
 * @brief Centralized compile-time configuration constants for the Smart Sensor Hub.
 * @ingroup app
 * @{
 */

/**
 * @name Sensor sampling periods per power mode
 * @brief Logical sampling intervals that depend on the current power mode.
 * @{
 */

/** @brief Sensor sampling period in ACTIVE mode (ms). */
#define SENSOR_PERIOD_ACTIVE_MS   (1000U)    /**< 1 second in ACTIVE mode.   */

/** @brief Sensor sampling period in IDLE mode (ms). */
#define SENSOR_PERIOD_IDLE_MS     (5000U)    /**< 5 seconds in IDLE mode.    */

/** @brief Sensor sampling period in SLEEP mode (ms). */
#define SENSOR_PERIOD_SLEEP_MS    (30000U)   /**< 30 seconds in SLEEP mode.  */

/**
 * @brief Sensor sampling period in STOP mode (ms).
 *
 * A value of 0 disables sampling in this mode.
 */
#define SENSOR_PERIOD_STOP_MS     (0U)       /**< 0 => no sampling in STOP.  */

/** @} */ /* end of Sensor sampling periods group */
/** @} */ /* end of app_config group */

#endif /* APP_CONFIG_H_ */
