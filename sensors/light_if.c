/**
 * @file light_if.c
 * @brief Light sensor interface selector (simulated vs hardware TSL2591).
 */

#include "light_if.h"
#include "app_config.h"
#include "log.h"
#include "light_sim_tsl2591.h"
#include "light_tsl2591.h"

/** @brief Backend function table. */
typedef struct
{
    bool (*init)(void);
    bool (*read)(TSL2591_Data_t *out);
} LightBackend_t;

/* Forward declarations for backend wrappers. */
static bool prv_sim_init(void);
static bool prv_sim_read(TSL2591_Data_t *out);
static bool prv_hw_init(void);
static bool prv_hw_read(TSL2591_Data_t *out);

/* Backend descriptors. */
static const LightBackend_t s_simBackend =
{
    .init = prv_sim_init,
    .read = prv_sim_read
};

static const LightBackend_t s_hwBackend =
{
    .init = prv_hw_init,
    .read = prv_hw_read
};

/* Currently active backend pointer. */
static const LightBackend_t *s_activeBackend = NULL;

bool LightIF_Init(void)
{
#if (APP_USE_SIM_LIGHT == 1U)
    s_activeBackend = &s_simBackend;
    LOG_INFO("LightIF_Init: using SIMULATED light backend.");
#elif (APP_USE_HW_LIGHT == 1U)
    s_activeBackend = &s_hwBackend;
    LOG_INFO("LightIF_Init: using HARDWARE TSL2591 backend.");
#else
#error "Exactly one of APP_USE_SIM_LIGHT or APP_USE_HW_LIGHT must be 1."
#endif

    if ((s_activeBackend == NULL) || (s_activeBackend->init == NULL))
    {
        LOG_ERROR("LightIF_Init: active backend invalid.");
        return false;
    }

    if (!s_activeBackend->init())
    {
        LOG_ERROR("LightIF_Init: backend init failed.");
        return false;
    }

    return true;
}

bool LightIF_Read(TSL2591_Data_t *out)
{
    if ((s_activeBackend == NULL) || (s_activeBackend->read == NULL))
    {
        LOG_ERROR("LightIF_Read: backend not initialized.");
        return false;
    }

    return s_activeBackend->read(out);
}

/* ------------------------------------------------------------------------- */
/* Backend wrappers                                                           */
/* ------------------------------------------------------------------------- */

static bool prv_sim_init(void)
{
    return LightSim_TSL2591_Init();
}

static bool prv_sim_read(TSL2591_Data_t *out)
{
    return LightSim_TSL2591_Read(out);
}

static bool prv_hw_init(void)
{
    /* Choose default IT / gain here. */
    return TSL2591_Init(TSL2591_IT_200MS, TSL2591_GAIN_MEDIUM);
}

static bool prv_hw_read(TSL2591_Data_t *out)
{
    return TSL2591_Read(out);
}
