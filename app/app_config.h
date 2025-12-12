/**
 * @file app_config.h
 * @brief Application-wide configuration constants for the Smart Sensor Hub.
 * @ingroup app
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup app_config Application Configuration
 * @brief Centralized compile-time configuration constants for the Smart Sensor Hub.
 * @ingroup app
 * @{
 */

/** @brief Use simulated environment (BME280) backend (1 = yes, 0 = no). */
#define APP_USE_SIM_ENV     (1U)

/** @brief Use hardware BME280 SPI backend (1 = yes, 0 = no). */
#define APP_USE_HW_ENV      (0U)

#if ((APP_USE_SIM_ENV + APP_USE_HW_ENV) != 1)
#error "Exactly one of APP_USE_SIM_ENV or APP_USE_HW_ENV must be set to 1."
#endif

/**
 * @brief Select which sensor backend to use.
 *
 * Exactly one of these should be set to 1.
 */
#define APP_USE_SIMULATED_SENSOR   (1U)
#define APP_USE_HW_SENSOR          (0U)

#if ((APP_USE_SIMULATED_SENSOR + APP_USE_HW_SENSOR) != 1)
#error "Exactly one of APP_USE_SIM_LIGHT or APP_USE_HW_LIGHT must be set to 1."
#endif
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

/** @brief Use simulated TSL2591 light sensor backend (1 = yes, 0 = no). */
#define APP_USE_SIM_LIGHT     (1U)

/** @brief Use hardware TSL2591 light sensor backend (1 = yes, 0 = no). */
#define APP_USE_HW_LIGHT      (0U)

#if ((APP_USE_SIM_LIGHT + APP_USE_HW_LIGHT) != 1)
#error "Exactly one of APP_USE_SIM_LIGHT or APP_USE_HW_LIGHT must be set to 1."
#endif

/** @} */ /* end of app_config group */

#endif /* APP_CONFIG_H_ */
