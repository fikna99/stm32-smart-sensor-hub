/**
 * @file sensor_hw_temp.c
 * @brief Hardware-backed temperature sensor using I2C.
 *
 * This implementation assumes a simple I2C temperature sensor with:
 *  - 7-bit address 0x48
 *  - temperature register at 0x00
 * and 12-bit temperature data similar to LM75/TMP102-style devices.
 *
 * Adapt the address/format if you use a different sensor.
 */

#include "sensor_hw_temp.h"
#include "app_config.h"
#include "log.h"
#include "main.h"           /* For hi2c1 declaration from CubeIDE. */
#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

/* ------------------------------------------------------------------------- */
/*  Configuration                                                            */
/* ------------------------------------------------------------------------- */

/** @brief 7-bit I2C address of the temperature sensor (0x48 is common). */
#define SENSOR_HW_I2C_ADDR_7BIT      (0x48U)

/** @brief 8-bit address (left-shifted) as used by STM32 HAL. */
#define SENSOR_HW_I2C_ADDR_8BIT      ((SENSOR_HW_I2C_ADDR_7BIT) << 1U)

/** @brief Temperature register address. */
#define SENSOR_HW_REG_TEMP           (0x00U)

/** @brief I2C timeout in milliseconds. */
#define SENSOR_HW_I2C_TIMEOUT_MS     (50U)

/* ------------------------------------------------------------------------- */
/*  Local functions                                                          */
/* ------------------------------------------------------------------------- */

static bool SensorHWTemp_Init(void);
static bool SensorHWTemp_Read(SensorData_t *out);

/* ------------------------------------------------------------------------- */
/*  Interface instance                                                       */
/* ------------------------------------------------------------------------- */

static const SensorIF_t s_hwSensorIF =
{
    .init = SensorHWTemp_Init,
    .read = SensorHWTemp_Read
};

const SensorIF_t * SensorHWTemp_GetInterface(void)
{
    return &s_hwSensorIF;
}

/* ------------------------------------------------------------------------- */
/*  Implementation                                                           */
/* ------------------------------------------------------------------------- */

/**
 * @brief Initialize the hardware sensor.
 *
 * For simple sensors this might be a no-op, but we can:
 *  - verify I2C is ready
 *  - read a register to confirm presence
 *
 * @retval true  Initialization (or basic presence check) succeeded.
 * @retval false Initialization failed.
 */
static bool SensorHWTemp_Init(void)
{
    HAL_StatusTypeDef status;
    uint8_t rx[2] = {0};

    LOG_INFO("SensorHWTemp_Init: using I2C addr 0x%02X", SENSOR_HW_I2C_ADDR_7BIT);

    status = HAL_I2C_Mem_Read(&hi2c1,
                              SENSOR_HW_I2C_ADDR_8BIT,
                              SENSOR_HW_REG_TEMP,
                              I2C_MEMADD_SIZE_8BIT,
                              rx,
                              sizeof(rx),
                              SENSOR_HW_I2C_TIMEOUT_MS);

    if (status != HAL_OK)
    {
        LOG_ERROR("SensorHWTemp_Init: I2C read failed (status=%d)", (int)status);
        return false;
    }

    LOG_INFO("SensorHWTemp_Init: initial temperature raw bytes: 0x%02X 0x%02X",
             rx[0], rx[1]);

    return true;
}

/**
 * @brief Read the current temperature from the hardware sensor.
 *
 * Expects a 12-bit temperature value in the top bits of the 2-byte register:
 *   raw = ((rx[0] << 8) | rx[1]) >> 4;
 *   temp_C = raw * 0.0625;
 *
 * @param[out] out Pointer to SensorData_t to be filled.
 *
 * @retval true  Read succeeded, @p out is valid.
 * @retval false Read failed.
 */
static bool SensorHWTemp_Read(SensorData_t *out)
{
    if (out == NULL)
    {
        return false;
    }

    uint8_t rx[2] = {0};
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c1,
                              SENSOR_HW_I2C_ADDR_8BIT,
                              SENSOR_HW_REG_TEMP,
                              I2C_MEMADD_SIZE_8BIT,
                              rx,
                              sizeof(rx),
                              SENSOR_HW_I2C_TIMEOUT_MS);
    if (status != HAL_OK)
    {
        LOG_ERROR("SensorHWTemp_Read: I2C read failed (status=%d)", (int)status);
        return false;
    }

    /* Convert 12-bit signed value to Celsius assuming 0.0625°C/LSB. */
    uint16_t raw12 = (uint16_t)(((uint16_t)rx[0] << 8U) | rx[1]);
    raw12 >>= 4;

    int16_t signed12 = (raw12 & 0x800U) ? (int16_t)(raw12 | 0xF000U) : (int16_t)raw12;

    float tempC = (float)signed12 * 0.0625f;

    out->value     = tempC;
    out->timestamp = Log_GetTimestampMs();
    out->mode      = 0U;  /* Optional: you can store PowerMode_t here if desired. */

    LOG_DEBUG("SensorHWTemp_Read: raw=0x%03X, temp=%.2f C", raw12, tempC);

    return true;
}
