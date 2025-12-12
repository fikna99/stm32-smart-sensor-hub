/**
 * @file env_if.c
 * @brief Environment backend selector implementation.
 */

#include "env_if.h"
#include "app_config.h"
#include "log.h"
#include "env_sim_bme280.h"
#include "env_bme280_spi.h"

/**
 * @brief Backend function table.
 */
typedef struct
{
    bool (*init)(void);
    bool (*read)(EnvData_t *out);
} EnvBackend_t;

static bool prv_sim_init(void);
static bool prv_sim_read(EnvData_t *out);
static bool prv_hw_init(void);
static bool prv_hw_read(EnvData_t *out);

static const EnvBackend_t s_simBackend =
{
    .init = prv_sim_init,
    .read = prv_sim_read
};

static const EnvBackend_t s_hwBackend =
{
    .init = prv_hw_init,
    .read = prv_hw_read
};

static const EnvBackend_t *s_activeBackend = NULL;

bool EnvIF_Init(void)
{
#if (APP_USE_SIM_ENV == 1U)
    s_activeBackend = &s_simBackend;
    LOG_INFO("EnvIF_Init: using SIMULATED environment backend.");
#elif (APP_USE_HW_ENV == 1U)
    s_activeBackend = &s_hwBackend;
    LOG_INFO("EnvIF_Init: using BME280 SPI hardware backend.");
#else
#error "Exactly one of APP_USE_SIM_ENV or APP_USE_HW_ENV must be 1."
#endif

    if ((s_activeBackend == NULL) || (s_activeBackend->init == NULL))
    {
        LOG_ERROR("EnvIF_Init: active backend invalid.");
        return false;
    }

    if (!s_activeBackend->init())
    {
        LOG_ERROR("EnvIF_Init: backend init failed.");
        return false;
    }

    return true;
}

bool EnvIF_Read(EnvData_t *out)
{
    if ((s_activeBackend == NULL) || (s_activeBackend->read == NULL))
    {
        LOG_ERROR("EnvIF_Read: backend not initialized.");
        return false;
    }

    return s_activeBackend->read(out);
}

/* ------------------------------------------------------------------------- */
/* Backend wrappers                                                          */
/* ------------------------------------------------------------------------- */

static bool prv_sim_init(void)
{
    return EnvSim_Init();
}

static bool prv_sim_read(EnvData_t *out)
{
    return EnvSim_Read(out);
}

static bool prv_hw_init(void)
{
    return EnvBME280_Init();
}

static bool prv_hw_read(EnvData_t *out)
{
    return EnvBME280_Read(out);
}
