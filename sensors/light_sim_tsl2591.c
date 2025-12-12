/**
 * @file light_sim_tsl2591.c
 * @brief Simulated TSL2591 light backend implementation.
 */

#include "light_sim_tsl2591.h"
#include "log.h"
#include <math.h>

/* Internal simulation state. */
static float   s_phase      = 0.0f;
static float   s_phaseStep  = 0.05f;
static uint8_t s_initialized = 0U;

bool LightSim_TSL2591_Init(void)
{
    s_phase       = 0.0f;
    s_phaseStep   = 0.05f;
    s_initialized = 1U;

    LOG_INFO("LightSim_TSL2591_Init: simulation backend ready.");
    return true;
}

bool LightSim_TSL2591_Read(TSL2591_Data_t *out)
{
    if (out == NULL)
    {
        return false;
    }

    if (s_initialized == 0U)
    {
        (void)LightSim_TSL2591_Init();
    }

    /* Generate a smooth pseudo-daylight waveform between ~10 and ~900 lux. */
    s_phase += s_phaseStep;
    if (s_phase > (2.0f * (float)M_PI))
    {
        s_phase -= 2.0f * (float)M_PI;
    }

    float baseLux = 450.0f + 440.0f * sinf(s_phase);
    if (baseLux < 0.0f)
    {
        baseLux = 0.0f;
    }

    /* Fake raw ADC counts roughly proportional to lux. */
    uint16_t full   = (uint16_t)(baseLux * 5.0f);
    uint16_t ir     = (uint16_t)(baseLux * 1.5f);

    out->lux         = baseLux;
    out->fullChannel = full;
    out->irChannel   = ir;
    out->timestampMs = Log_GetTimestampMs();

    LOG_DEBUG("LightSim: lux=%.2f, full=%u, ir=%u, t=%lu ms",
              (double)out->lux,
              (unsigned int)out->fullChannel,
              (unsigned int)out->irChannel,
              (unsigned long)out->timestampMs);

    return true;
}
