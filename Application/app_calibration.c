#include "app_calibration.h"
#include "bsp_at24c08.h"

#define CALIBRATION_STORAGE_ADDRESS 0U
#define CALIBRATION_STORAGE_VERSION 1U
#define CALIBRATION_STORAGE_SIZE    104U
#define CALIBRATION_STORAGE_CRC_POS 102U

static uint16_t Calibration_CalculateCrc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc;
    uint8_t bit_index;

    crc = 0xFFFFU;
    while (length > 0U)
    {
        crc ^= (uint16_t)*data++ << 8U;
        for (bit_index = 0U; bit_index < 8U; bit_index++)
        {
            crc = (crc & 0x8000U) != 0U ? (uint16_t)((crc << 1U) ^ 0x1021U) : (uint16_t)(crc << 1U);
        }
        length--;
    }

    return crc;
}

static void Calibration_StoreU16(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8U);
}

static void Calibration_StoreU32(uint8_t *data, uint32_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8U);
    data[2] = (uint8_t)(value >> 16U);
    data[3] = (uint8_t)(value >> 24U);
}

static uint16_t Calibration_LoadU16(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8U);
}

static uint32_t Calibration_LoadU32(const uint8_t *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8U) |
           ((uint32_t)data[2] << 16U) | ((uint32_t)data[3] << 24U);
}

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

CalibrationStorageStatus Calibration_Save(void)
{
    uint8_t storage[CALIBRATION_STORAGE_SIZE] = {0U};
    uint8_t count;
    uint8_t index;
    uint16_t crc;
    const CalibrationPoint *points;

    points = Calibration_GetTable(&count);
    if (count == 0U)
    {
        return CALIBRATION_STORAGE_STATUS_EMPTY;
    }

    storage[0] = 'P';
    storage[1] = 'C';
    storage[2] = 'A';
    storage[3] = 'L';
    storage[4] = CALIBRATION_STORAGE_VERSION;
    storage[5] = count;
    for (index = 0U; index < count; index++)
    {
        Calibration_StoreU32(&storage[6U + (uint16_t)index * 6U], points[index].frequency_hz);
        Calibration_StoreU16(&storage[10U + (uint16_t)index * 6U], points[index].paper_count);
    }

    crc = Calibration_CalculateCrc16(storage, CALIBRATION_STORAGE_CRC_POS);
    Calibration_StoreU16(&storage[CALIBRATION_STORAGE_CRC_POS], crc);

    return AT24C08_Write(CALIBRATION_STORAGE_ADDRESS, storage, CALIBRATION_STORAGE_SIZE) != 0U ?
           CALIBRATION_STORAGE_STATUS_OK : CALIBRATION_STORAGE_STATUS_EEPROM_ERROR;
}

CalibrationStorageStatus Calibration_Load(void)
{
    uint8_t storage[CALIBRATION_STORAGE_SIZE];
    CalibrationPoint points[CALIBRATION_MAX_POINTS];
    CalibrationStatus status;
    uint8_t count;
    uint8_t index;
    uint16_t stored_crc;

    if (AT24C08_Read(CALIBRATION_STORAGE_ADDRESS, storage, CALIBRATION_STORAGE_SIZE) == 0U)
    {
        return CALIBRATION_STORAGE_STATUS_EEPROM_ERROR;
    }
    if ((storage[0] != 'P') || (storage[1] != 'C') || (storage[2] != 'A') ||
        (storage[3] != 'L') || (storage[4] != CALIBRATION_STORAGE_VERSION))
    {
        return CALIBRATION_STORAGE_STATUS_FORMAT_ERROR;
    }

    stored_crc = Calibration_LoadU16(&storage[CALIBRATION_STORAGE_CRC_POS]);
    if (stored_crc != Calibration_CalculateCrc16(storage, CALIBRATION_STORAGE_CRC_POS))
    {
        return CALIBRATION_STORAGE_STATUS_CHECKSUM_ERROR;
    }

    count = storage[5];
    if ((count == 0U) || (count > CALIBRATION_MAX_POINTS))
    {
        return CALIBRATION_STORAGE_STATUS_FORMAT_ERROR;
    }
    for (index = 0U; index < count; index++)
    {
        points[index].frequency_hz = Calibration_LoadU32(&storage[6U + (uint16_t)index * 6U]);
        points[index].paper_count = Calibration_LoadU16(&storage[10U + (uint16_t)index * 6U]);
    }

    status = Calibration_SetTable(points, count);
    return status == CALIBRATION_STATUS_OK ? CALIBRATION_STORAGE_STATUS_OK : CALIBRATION_STORAGE_STATUS_TABLE_ERROR;
}
