/**
 * @file light_if.h
 * @brief Light sensor interface selector (simulated vs hardware TSL2591).
 *
 * This module chooses between a simulated light backend and the real
 * TSL2591 driver, based on APP_USE_SIM_LIGHT / APP_USE_HW_LIGHT in
 * app_config.h.
 */

#ifndef LIGHT_IF_H
#define LIGHT_IF_H

#include <stdbool.h>
#include "light_tsl2591.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize and select the active light sensor backend.
 *
 * This is called once from App_MainInit(). It:
 *  - chooses the backend (simulated or hardware)
 *  - calls its init() function
 *
 * @retval true  Light backend initialized successfully.
 * @retval false Initialization failed.
 */
bool LightIF_Init(void);

/**
 * @brief Read a single light sample from the active backend.
 *
 * @param[out] out Pointer to a TSL2591_Data_t structure to populate.
 *
 * @retval true  Sample read successfully.
 * @retval false Read failed or backend not initialized.
 */
bool LightIF_Read(TSL2591_Data_t *out);

#ifdef __cplusplus
}
#endif

#endif /* LIGHT_IF_H */
