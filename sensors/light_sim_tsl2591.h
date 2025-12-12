/**
 * @file light_sim_tsl2591.h
 * @brief Simulated TSL2591 light backend.
 *
 * This module provides a fake TSL2591-like backend so that the
 * application can be exercised without real hardware.
 */

#ifndef LIGHT_SIM_TSL2591_H
#define LIGHT_SIM_TSL2591_H

#include <stdbool.h>
#include "light_tsl2591.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the simulated light backend.
 *
 * @retval true  Always returns true for simulation.
 * @retval false Never used for simulation.
 */
bool LightSim_TSL2591_Init(void);

/**
 * @brief Generate a simulated light sample.
 *
 * This produces a deterministic, slowly varying lux waveform
 * so that logs are easy to read and reason about.
 *
 * @param[out] out Pointer to TSL2591_Data_t structure to populate.
 *
 * @retval true  Sample generated successfully.
 * @retval false out was NULL.
 */
bool LightSim_TSL2591_Read(TSL2591_Data_t *out);

#ifdef __cplusplus
}
#endif

#endif /* LIGHT_SIM_TSL2591_H */
