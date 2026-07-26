#include "app_calibration.h"

static CalibrationPoint calibration_table[CALIBRATION_MAX_POINTS];
static uint8_t calibration_point_count;

void Calibration_Clear(void)
{
    calibration_point_count = 0U;
}

CalibrationStatus Calibration_SetTable(const CalibrationPoint *points, uint8_t count)
{
    uint8_t index;

    if ((points == 0) || (count == 0U) || (count > CALIBRATION_MAX_POINTS))
    {
        return CALIBRATION_STATUS_INVALID_PARAMETER;
    }

    for (index = 1U; index < count; index++)
    {
        if (points[index].frequency_hz <= points[index - 1U].frequency_hz)
        {
            return CALIBRATION_STATUS_NOT_SORTED;
        }
    }

    for (index = 0U; index < count; index++)
    {
        calibration_table[index] = points[index];
    }

    calibration_point_count = count;
    return CALIBRATION_STATUS_OK;
}

const CalibrationPoint *Calibration_GetTable(uint8_t *count)
{
    if (count != 0)
    {
        *count = calibration_point_count;
    }

    return calibration_table;
}

CalibrationStatus Calibration_ConvertFrequency(uint32_t frequency_hz, uint16_t *paper_count)
{
    uint8_t index;
    uint32_t frequency_span;
    uint32_t frequency_offset;
    int64_t count_span;
    int64_t interpolated_count;

    if (paper_count == 0)
    {
        return CALIBRATION_STATUS_INVALID_PARAMETER;
    }

    if (calibration_point_count == 0U)
    {
        return CALIBRATION_STATUS_EMPTY;
    }

    if (frequency_hz < calibration_table[0].frequency_hz)
    {
        return CALIBRATION_STATUS_OUT_OF_RANGE_LOW;
    }

    if (frequency_hz > calibration_table[calibration_point_count - 1U].frequency_hz)
    {
        return CALIBRATION_STATUS_OUT_OF_RANGE_HIGH;
    }

    for (index = 0U; index < calibration_point_count; index++)
    {
        if (frequency_hz == calibration_table[index].frequency_hz)
        {
            *paper_count = calibration_table[index].paper_count;
            return CALIBRATION_STATUS_OK;
        }
    }

    for (index = 0U; index < (calibration_point_count - 1U); index++)
    {
        if (frequency_hz < calibration_table[index + 1U].frequency_hz)
        {
            frequency_span = calibration_table[index + 1U].frequency_hz - calibration_table[index].frequency_hz;
            frequency_offset = frequency_hz - calibration_table[index].frequency_hz;
            count_span = (int64_t)calibration_table[index + 1U].paper_count - (int64_t)calibration_table[index].paper_count;
            interpolated_count = (int64_t)calibration_table[index].paper_count + (count_span * (int64_t)frequency_offset) / (int64_t)frequency_span;

            *paper_count = (uint16_t)interpolated_count;
            return CALIBRATION_STATUS_OK;
        }
    }

    return CALIBRATION_STATUS_INVALID_PARAMETER;
}
