/**
 * @file power_manager.h
 * @brief Power management framework for Smart Sensor Hub.
 *
 * This module provides a central place to track the system power mode,
 * implement simple policies, and eventually integrate STM32 low-power
 * features (sleep/stop modes, wake-up sources, etc.).
 *
 * @ingroup power
 */

#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup power Power Management
 * @brief High-level power mode management for the Smart Sensor Hub.
 * @ingroup app
 * @{
 */

/**
 * @brief High-level power modes for the system.
 *
 * The enumeration is intentionally abstract and decoupled from specific
 * STM32 low-power modes. Later phases may map these to SLEEP/STOP/STANDBY.
 */
typedef enum
{
    POWER_MODE_ACTIVE = 0U,  /**< Full-speed operation, all tasks running. */
    POWER_MODE_IDLE,         /**< Reduced activity, only essential tasks.  */
    POWER_MODE_SLEEP,        /**< Light sleep; quick wake-up expected.     */
    POWER_MODE_STOP          /**< Deeper low-power state (future use).     */
} PowerMode_t;

/**
 * @brief Initialize the power manager module.
 *
 * This function resets internal state and sets the initial power mode
 * to POWER_MODE_ACTIVE. It should be called once during system startup
 * before the Power Manager task begins executing.
 *
 * @return None.
 */
void PowerManager_Init(void);

/**
 * @brief Request a transition to a new power mode.
 *
 * This function records the requested power mode. The actual transition
 * decision may be made later in PowerManager_Update(), based on system
 * policies or constraints.
 *
 * @param mode Desired power mode.
 *
 * @return None.
 */
void PowerManager_RequestMode(PowerMode_t mode);

/**
 * @brief Get the currently active power mode.
 *
 * @return The current power mode.
 */
PowerMode_t PowerManager_GetCurrentMode(void);

/**
 * @brief Periodic update function for the Power Manager.
 *
 * This function is intended to be called from a scheduled task at a
 * relatively low frequency (e.g. every 500 ms). It evaluates the current
 * requests and policies and, if needed, triggers logging or future
 * low-power transitions.
 *
 * In Phase 3, it only logs state changes; actual low-power entry will
 * be implemented in later phases.
 *
 * @return None.
 */
void PowerManager_Update(void);

/** @} */ /* end of power group */

#ifdef __cplusplus
}
#endif

#endif /* POWER_MANAGER_H */
