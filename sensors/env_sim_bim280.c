/**
 * @file env_sim_bme280.c
 * @brief Simulated environment backend implementation.
 */

#include "env_sim_bme280.h"
#include "log.h"
#include <math.h>

/* Avoid M_PI portability issues. */
#define ENV_SIM_PI  (3.14159265358979323846f)

static float   s_phase       = 0.0f;
static float   s_phaseStep   = 0.02f;
static uint8_t s_initialized = 0U;

bool EnvSim_Init(void)
{
    s_phase       = 0.0f;
    s_phaseStep   = 0.02f;
    s_initialized = 1U;

    LOG_INFO("EnvSim_Init: simulated environment backend ready.");
    return true;
}

bool EnvSim_Read(EnvData_t *out)
{
    if (out == NULL)
    {
        return false;
    }

    if (s_initialized == 0U)
    {
        (void)EnvSim_Init();
    }

    s_phase += s_phaseStep;
    if (s_phase > (2.0f * ENV_SIM_PI))
    {
        s_phase -= 2.0f * ENV_SIM_PI;
    }

    /* Smooth realistic ranges. */
    out->temperatureC = 24.0f + 4.0f * sinf(s_phase);                 /* 20..28 C */
    out->pressurePa   = 101325.0f + 800.0f * sinf(s_phase * 0.5f);    /* ~100.5..102.1 kPa */
    out->humidityRH   = 52.0f + 12.0f * sinf(s_phase * 0.8f + 1.0f);  /* ~40..65 % */

    out->timestampMs  = Log_GetTimestampMs();

    LOG_DEBUG("EnvSim: T=%.2fC P=%.2fPa H=%.2f%% t=%lu",
              (double)out->temperatureC,
              (double)out->pressurePa,
              (double)out->humidityRH,
              (unsigned long)out->timestampMs);

    return true;
}
