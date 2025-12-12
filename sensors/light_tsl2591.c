/**
 * @file light_tsl2591.c
 * @brief Driver implementation for the TSL2591 ambient light sensor.
 */

#include "light_tsl2591.h"
#include "log.h"
#include "main.h"      /* For I2C handle declaration. */

#include "stm32f4xx_hal.h"
#include <math.h>

/* I2C 7-bit address (datasheet, primary address 0x29). */
#define TSL2591_I2C_ADDR_7BIT      (0x29U)
#define TSL2591_I2C_ADDR_8BIT      (TSL2591_I2C_ADDR_7BIT << 1)

/* Command register prefix: CMD=1, TRANSACTION=01 (normal). See datasheet. */
#define TSL2591_CMD_NORMAL         (0xA0U)

/* Register addresses (without command prefix). See datasheet "Register Address Map". */
#define TSL2591_REG_ENABLE         (0x00U)
#define TSL2591_REG_CONTROL        (0x01U)
#define TSL2591_REG_PERSIST        (0x0CU)
#define TSL2591_REG_DEVICE_ID      (0x12U)
#define TSL2591_REG_STATUS         (0x13U)
#define TSL2591_REG_C0DATAL        (0x14U)
#define TSL2591_REG_C1DATAL        (0x16U)

/* ENABLE register bits. */
#define TSL2591_ENABLE_PON         (1U << 0)  /**< Power ON. */
#define TSL2591_ENABLE_AEN         (1U << 1)  /**< ALS enable. */

/* STATUS register bits. */
#define TSL2591_STATUS_AVALID      (1U << 0)  /**< ALS data valid. */

/* Expected device ID value from datasheet (ID = 0x50). */
#define TSL2591_DEVICE_ID_EXPECTED (0x50U)

/* Lux calculation constants (approximate, see datasheet Application Info). */
#define TSL2591_LUX_DF             (408.0f)   /**< Device-specific lux divisor factor. */

/* Forward declaration of I2C handle exported from main.c (CubeMX). */
extern I2C_HandleTypeDef hi2c1;

/* Static driver context. */
static I2C_HandleTypeDef         *s_i2c  = NULL;
static TSL2591_IntegrationTime_t  s_it   = TSL2591_IT_200MS;
static TSL2591_Gain_t             s_gain = TSL2591_GAIN_MEDIUM;

/* Local helper prototypes. */
static bool  prv_write_u8(uint8_t reg, uint8_t value);
static bool  prv_read_u8(uint8_t reg, uint8_t *value);
static bool  prv_read_u16(uint8_t regLow, uint16_t *value);
static bool  prv_wait_data_valid(uint32_t timeoutMs);
static float prv_compute_lux(uint16_t full, uint16_t ir);

/* ------------------------------------------------------------------------- */

bool TSL2591_Init(TSL2591_IntegrationTime_t it, TSL2591_Gain_t gain)
{
    uint8_t id = 0U;
    uint8_t control = 0U;

    s_i2c  = &hi2c1;
    s_it   = it;
    s_gain = gain;

    /* Read and check device ID. */
    if (!prv_read_u8(TSL2591_REG_DEVICE_ID, &id))
    {
        LOG_ERROR("TSL2591_Init: failed to read ID register.");
        return false;
    }

    if (id != TSL2591_DEVICE_ID_EXPECTED)
    {
        LOG_ERROR("TSL2591_Init: unexpected ID = 0x%02X (expected 0x%02X).",
                  (unsigned int)id, (unsigned int)TSL2591_DEVICE_ID_EXPECTED);
        /* Report failure so caller can decide what to do. */
        return false;
    }

    /* Configure CONTROL register: AGAIN[5:4] + ATIME[2:0]. */
    control = (uint8_t)(((uint8_t)gain << 4) | ((uint8_t)it & 0x07U));
    if (!prv_write_u8(TSL2591_REG_CONTROL, control))
    {
        LOG_ERROR("TSL2591_Init: failed to write CONTROL.");
        return false;
    }

    /* Optional: set interrupt persistence to "any out of range" (not used yet). */
    (void)prv_write_u8(TSL2591_REG_PERSIST, 0x01U);

    /* Power ON + enable ALS. */
    if (!prv_write_u8(TSL2591_REG_ENABLE, (uint8_t)(TSL2591_ENABLE_PON | TSL2591_ENABLE_AEN)))
    {
        LOG_ERROR("TSL2591_Init: failed to write ENABLE.");
        return false;
    }

    LOG_INFO("TSL2591_Init: ID=0x%02X, it=%u, gain=%u.",
             (unsigned int)id, (unsigned int)it, (unsigned int)gain);

    return true;
}

bool TSL2591_Read(TSL2591_Data_t *out)
{
    uint8_t status = 0U;
    uint16_t ch0   = 0U;
    uint16_t ch1   = 0U;

    if ((out == NULL) || (s_i2c == NULL))
    {
        return false;
    }

    /* Wait for ALS data to be valid (AVALID bit). */
    if (!prv_wait_data_valid(1000U))
    {
        LOG_WARN("TSL2591_Read: timeout waiting for AVALID.");
        return false;
    }

    /* Read both channels. Read CH0 first, then CH1 as per datasheet. */
    if (!prv_read_u16(TSL2591_REG_C0DATAL, &ch0))
    {
        LOG_ERROR("TSL2591_Read: failed to read CH0.");
        return false;
    }

    if (!prv_read_u16(TSL2591_REG_C1DATAL, &ch1))
    {
        LOG_ERROR("TSL2591_Read: failed to read CH1.");
        return false;
    }

    /* Optional: read STATUS to clear AVALID (depending on configuration). */
    (void)prv_read_u8(TSL2591_REG_STATUS, &status);
    (void)status;

    out->fullChannel  = ch0;
    out->irChannel    = ch1;
    out->lux          = prv_compute_lux(ch0, ch1);
    out->timestampMs  = Log_GetTimestampMs();

    LOG_DEBUG("TSL2591_Read: CH0=%u, CH1=%u, lux=%.2f.",
              (unsigned int)ch0, (unsigned int)ch1, (double)out->lux);

    return true;
}

/* ------------------------------------------------------------------------- */
/* Local helpers                                                             */
/* ------------------------------------------------------------------------- */

static bool prv_write_u8(uint8_t reg, uint8_t value)
{
    HAL_StatusTypeDef st;

    if (s_i2c == NULL)
    {
        return false;
    }

    uint8_t addr = (uint8_t)(TSL2591_CMD_NORMAL | (reg & 0x1FU));
    st = HAL_I2C_Mem_Write(s_i2c,
                           TSL2591_I2C_ADDR_8BIT,
                           addr,
                           I2C_MEMADD_SIZE_8BIT,
                           &value,
                           1U,
                           50U);
    if (st != HAL_OK)
    {
        LOG_WARN("TSL2591: I2C write failed reg=0x%02X status=%d.",
                 (unsigned int)reg, (int)st);
        return false;
    }

    return true;
}

static bool prv_read_u8(uint8_t reg, uint8_t *value)
{
    HAL_StatusTypeDef st;

    if ((s_i2c == NULL) || (value == NULL))
    {
        return false;
    }

    uint8_t addr = (uint8_t)(TSL2591_CMD_NORMAL | (reg & 0x1FU));
    st = HAL_I2C_Mem_Read(s_i2c,
                          TSL2591_I2C_ADDR_8BIT,
                          addr,
                          I2C_MEMADD_SIZE_8BIT,
                          value,
                          1U,
                          50U);
    if (st != HAL_OK)
    {
        LOG_WARN("TSL2591: I2C read8 failed reg=0x%02X status=%d.",
                 (unsigned int)reg, (int)st);
        return false;
    }

    return true;
}

static bool prv_read_u16(uint8_t regLow, uint16_t *value)
{
    HAL_StatusTypeDef st;
    uint8_t buf[2] = {0U, 0U};

    if ((s_i2c == NULL) || (value == NULL))
    {
        return false;
    }

    uint8_t addr = (uint8_t)(TSL2591_CMD_NORMAL | (regLow & 0x1FU));
    st = HAL_I2C_Mem_Read(s_i2c,
                          TSL2591_I2C_ADDR_8BIT,
                          addr,
                          I2C_MEMADD_SIZE_8BIT,
                          buf,
                          2U,
                          50U);
    if (st != HAL_OK)
    {
        LOG_WARN("TSL2591: I2C read16 failed reg=0x%02X status=%d.",
                 (unsigned int)regLow, (int)st);
        return false;
    }

    *value = (uint16_t)((((uint16_t)buf[1]) << 8) | (uint16_t)buf[0]);
    return true;
}

static bool prv_wait_data_valid(uint32_t timeoutMs)
{
    uint32_t start = Log_GetTimestampMs();
    uint8_t status = 0U;

    while ((Log_GetTimestampMs() - start) < timeoutMs)
    {
        if (!prv_read_u8(TSL2591_REG_STATUS, &status))
        {
            return false;
        }

        if ((status & TSL2591_STATUS_AVALID) != 0U)
        {
            return true;
        }
    }

    return false;
}

static float prv_compute_lux(uint16_t full, uint16_t ir)
{
    /* Derived from datasheet application section.
     * This is an approximate calculation suitable for demo / bring-up.
     * For production designs, consult the datasheet and calibrate.
     */

    float atimeMs = 0.0f;
    float again   = 1.0f;

    switch (s_it)
    {
        case TSL2591_IT_100MS: atimeMs = 100.0f; break;
        case TSL2591_IT_200MS: atimeMs = 200.0f; break;
        case TSL2591_IT_300MS: atimeMs = 300.0f; break;
        case TSL2591_IT_400MS: atimeMs = 400.0f; break;
        case TSL2591_IT_500MS: atimeMs = 500.0f; break;
        case TSL2591_IT_600MS: atimeMs = 600.0f; break;
        default:               atimeMs = 100.0f; break;
    }

    switch (s_gain)
    {
        case TSL2591_GAIN_LOW:    again = 1.0f;   break;
        case TSL2591_GAIN_MEDIUM: again = 25.0f;  break;
        case TSL2591_GAIN_HIGH:   again = 428.0f; break;
        case TSL2591_GAIN_MAX:    again = 9876.0f; break;
        default:                  again = 1.0f;   break;
    }

    /* Compute "counts per lux" factor. */
    float cpl = (atimeMs * again) / TSL2591_LUX_DF;
    if (cpl <= 0.0f)
    {
        return 0.0f;
    }

    /* Two separate approximations (typical in many example drivers). */
    float lux1 = ((float)full - (1.87f * (float)ir)) / cpl;
    float lux2 = ((0.63f * (float)full) - (float)ir) / cpl;

    float lux = lux1;
    if (lux2 > lux)
    {
        lux = lux2;
    }

    if (lux < 0.0f)
    {
        lux = 0.0f;
    }

    return lux;
}

