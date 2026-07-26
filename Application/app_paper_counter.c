#include "app_paper_counter.h"
#include "app_calibration.h"

static PaperCounterConfig paper_counter_config;
static uint32_t stable_frequency_hz;
static uint8_t stable_sample_count;
static uint8_t paper_counter_initialized;

static uint32_t PaperCounter_GetFrequencyDifference(uint32_t first, uint32_t second)
{
    if (first >= second)
    {
        return first - second;
    }

    return second - first;
}

static PaperCounterStatus PaperCounter_ConvertCalibrationStatus(CalibrationStatus status)
{
    if (status == CALIBRATION_STATUS_EMPTY)
    {
        return PAPER_COUNTER_STATUS_CALIBRATION_EMPTY;
    }

    if (status == CALIBRATION_STATUS_OUT_OF_RANGE_LOW)
    {
        return PAPER_COUNTER_STATUS_FREQUENCY_LOW;
    }

    if (status == CALIBRATION_STATUS_OUT_OF_RANGE_HIGH)
    {
        return PAPER_COUNTER_STATUS_FREQUENCY_HIGH;
    }

    return PAPER_COUNTER_STATUS_CALIBRATION_ERROR;
}

PaperCounterStatus PaperCounter_Init(const PaperCounterConfig *config)
{
    if ((config == 0) || (config->required_stable_samples == 0U))
    {
        return PAPER_COUNTER_STATUS_INVALID_PARAMETER;
    }

    paper_counter_config = *config;
    paper_counter_initialized = 1U;
    PaperCounter_Reset();

    return PAPER_COUNTER_STATUS_OK;
}

void PaperCounter_Reset(void)
{
    stable_frequency_hz = 0U;
    stable_sample_count = 0U;
}

PaperCounterStatus PaperCounter_ProcessFrequency(uint32_t frequency_hz, uint16_t *paper_count)
{
    CalibrationStatus calibration_status;

    if (paper_count == 0)
    {
        return PAPER_COUNTER_STATUS_INVALID_PARAMETER;
    }

    if (paper_counter_initialized == 0U)
    {
        return PAPER_COUNTER_STATUS_NOT_INITIALIZED;
    }

    if (stable_sample_count == 0U)
    {
        stable_frequency_hz = frequency_hz;
        stable_sample_count = 1U;
    }
    else if (PaperCounter_GetFrequencyDifference(frequency_hz, stable_frequency_hz) <= paper_counter_config.frequency_tolerance_hz)
    {
        if (stable_sample_count < paper_counter_config.required_stable_samples)
        {
            stable_frequency_hz = (uint32_t)(((uint64_t)stable_frequency_hz * (uint64_t)stable_sample_count + (uint64_t)frequency_hz) /
                                             (uint64_t)(stable_sample_count + 1U));
            stable_sample_count++;
        }
    }
    else
    {
        stable_frequency_hz = frequency_hz;
        stable_sample_count = 1U;
    }

    if (stable_sample_count < paper_counter_config.required_stable_samples)
    {
        return PAPER_COUNTER_STATUS_WAITING_FOR_STABLE;
    }

    calibration_status = Calibration_ConvertFrequency(stable_frequency_hz, paper_count);
    if (calibration_status == CALIBRATION_STATUS_OK)
    {
        return PAPER_COUNTER_STATUS_OK;
    }

    return PaperCounter_ConvertCalibrationStatus(calibration_status);
}
