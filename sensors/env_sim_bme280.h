/**
 * @file env_sim_bme280.h
 * @brief Simulated environment backend (BME280-like).
 */

#ifndef ENV_SIM_BME280_H
#define ENV_SIM_BME280_H

#include <stdbool.h>
#include "env_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool EnvSim_Init(void);
bool EnvSim_Read(EnvData_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ENV_SIM_BME280_H */
