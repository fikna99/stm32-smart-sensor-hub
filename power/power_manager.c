/**
 * @file power_manager.c
 * @brief Implementation of the power management framework.
 */

#include "power_manager.h"
#include "log.h"

/**
 * @brief Current power mode.
 */
static PowerMode_t s_currentMode = POWER_MODE_ACTIVE;

/**
 * @brief Last requested power mode.
 */
static PowerMode_t s_requestedMode = POWER_MODE_ACTIVE;

/**
 * @brief Number of update cycles spent idle.
 *
 * This can later be used to trigger automatic transitions to lower
 * power modes after prolonged inactivity.
 */
static uint32_t s_idleCycles = 0U;

/* ------------------------------------------------------------------------- */

void PowerManager_Init(void)
{
    s_currentMode   = POWER_MODE_ACTIVE;
    s_requestedMode = POWER_MODE_ACTIVE;
    s_idleCycles    = 0U;

    LOG_INFO("PowerManager: initialized (mode = ACTIVE)");
}

void PowerManager_RequestMode(PowerMode_t mode)
{
    if (mode != s_requestedMode)
    {
        s_requestedMode = mode;
        LOG_INFO("PowerManager: requested mode change to %d", (int)mode);
    }
}

PowerMode_t PowerManager_GetCurrentMode(void)
{
    return s_currentMode;
}

void PowerManager_Update(void)
{
    /* In Phase 3, we limit ourselves to logging state transitions and
     * maintaining a simple idea of "idle cycles". Later phases may use
     * this function to actually enter STM32 low-power modes.
     */

    if (s_requestedMode != s_currentMode)
    {
        LOG_INFO("PowerManager: applying mode change %d -> %d",
                 (int)s_currentMode,
                 (int)s_requestedMode);

        s_currentMode = s_requestedMode;
        s_idleCycles  = 0U;
    }
    else
    {
        /* No change requested; consider this an "idle" check-in. */
        s_idleCycles++;
        LOG_DEBUG("PowerManager: mode=%d, idleCycles=%lu",
                  (int)s_currentMode,
                  (unsigned long)s_idleCycles);
    }

    /* Future work (later phases):
     * - If mode == POWER_MODE_SLEEP or POWER_MODE_STOP:
     *     - Coordinate with main loop or RTOS to enter low-power.
     *     - Configure wake-up sources (timers, GPIO, etc.).
     * - Adjust sampling rates based on power mode.
     */
}
