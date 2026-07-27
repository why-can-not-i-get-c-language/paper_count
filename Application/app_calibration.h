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

typedef enum
{
    CALIBRATION_STORAGE_STATUS_OK = 0,
    CALIBRATION_STORAGE_STATUS_EMPTY,
    CALIBRATION_STORAGE_STATUS_EEPROM_ERROR,
    CALIBRATION_STORAGE_STATUS_FORMAT_ERROR,
    CALIBRATION_STORAGE_STATUS_CHECKSUM_ERROR,
    CALIBRATION_STORAGE_STATUS_TABLE_ERROR
} CalibrationStorageStatus;

/* 清空当前标定表。 */
void Calibration_Clear(void);

/* 设置完整标定表，频率必须严格按升序排列。 */
CalibrationStatus Calibration_SetTable(const CalibrationPoint *points, uint8_t count);

/* 获取当前标定表，供 EEPROM 保存和界面显示使用。 */
const CalibrationPoint *Calibration_GetTable(uint8_t *count);

/* 根据频率查询纸张数量，区间内使用整数分段线性插值。 */
CalibrationStatus Calibration_ConvertFrequency(uint32_t frequency_hz, uint16_t *paper_count);

/* 将当前标定表保存到 AT24C08。 */
CalibrationStorageStatus Calibration_Save(void);

/* 从 AT24C08 加载并校验标定表。 */
CalibrationStorageStatus Calibration_Load(void);

/* 基于当前正式标定表开始一次可取消的编辑会话。 */
void Calibration_BeginEdit(void);

/* 放弃未保存编辑，恢复为当前正式标定表。 */
void Calibration_DiscardEdit(void);

/* 为指定纸张张数新增或更新一个编辑标定点，并按频率升序整理。 */
CalibrationStatus Calibration_SetEditPoint(uint16_t paper_count, uint32_t frequency_hz);

/* 获取正在编辑的标定表。 */
const CalibrationPoint *Calibration_GetEditTable(uint8_t *count);

/* 保存编辑标定表；成功后将其作为当前正式标定表。 */
CalibrationStorageStatus Calibration_SaveEdit(void);

#endif
