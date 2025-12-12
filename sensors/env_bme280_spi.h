/**
 * @file env_bme280_spi.h
 * @brief BME280 driver over SPI (blocking HAL).
 */

#ifndef ENV_BME280_SPI_H
#define ENV_BME280_SPI_H

#include <stdbool.h>
#include "env_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool EnvBME280_Init(void);
bool EnvBME280_Read(EnvData_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ENV_BME280_SPI_H */
