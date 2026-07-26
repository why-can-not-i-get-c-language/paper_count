#ifndef __APP_CALIBRATION_H
#define __APP_CALIBRATION_H

#include <stdint.h>

#define CALIBRATION_MAX_POINTS 16U

typedef struct
{
    uint32_t frequency_hz;
    uint16_t paper_count;
} CalibrationPoint;

typedef enum
{
    CALIBRATION_STATUS_OK = 0,
    CALIBRATION_STATUS_INVALID_PARAMETER,
    CALIBRATION_STATUS_EMPTY,
    CALIBRATION_STATUS_NOT_SORTED,
    CALIBRATION_STATUS_OUT_OF_RANGE_LOW,
    CALIBRATION_STATUS_OUT_OF_RANGE_HIGH
} CalibrationStatus;

/* 清空当前标定表。 */
void Calibration_Clear(void);

/* 设置完整标定表，频率必须严格按升序排列。 */
CalibrationStatus Calibration_SetTable(const CalibrationPoint *points, uint8_t count);

/* 获取当前标定表，供 EEPROM 保存和界面显示使用。 */
const CalibrationPoint *Calibration_GetTable(uint8_t *count);

/* 根据频率查询纸张数量，区间内使用整数分段线性插值。 */
CalibrationStatus Calibration_ConvertFrequency(uint32_t frequency_hz, uint16_t *paper_count);

#endif
