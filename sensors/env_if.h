/**
 * @file env_if.h
 * @brief Environment interface selector (simulated vs BME280 SPI).
 */

#ifndef ENV_IF_H
#define ENV_IF_H

#include <stdbool.h>
#include "env_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the active environment backend.
 */
bool EnvIF_Init(void);

/**
 * @brief Read a single environment sample from the active backend.
 */
bool EnvIF_Read(EnvData_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ENV_IF_H */
