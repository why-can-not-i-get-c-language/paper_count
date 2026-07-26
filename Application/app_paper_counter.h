#ifndef __APP_PAPER_COUNTER_H
#define __APP_PAPER_COUNTER_H

#include <stdint.h>

typedef struct
{
    uint32_t frequency_tolerance_hz;
    uint8_t required_stable_samples;
} PaperCounterConfig;

typedef enum
{
    PAPER_COUNTER_STATUS_OK = 0,
    PAPER_COUNTER_STATUS_WAITING_FOR_STABLE,
    PAPER_COUNTER_STATUS_INVALID_PARAMETER,
    PAPER_COUNTER_STATUS_NOT_INITIALIZED,
    PAPER_COUNTER_STATUS_CALIBRATION_EMPTY,
    PAPER_COUNTER_STATUS_FREQUENCY_LOW,
    PAPER_COUNTER_STATUS_FREQUENCY_HIGH,
    PAPER_COUNTER_STATUS_CALIBRATION_ERROR
} PaperCounterStatus;

/* 配置稳定判定参数并清除历史采样数据。 */
PaperCounterStatus PaperCounter_Init(const PaperCounterConfig *config);

/* 清除当前稳定判定状态，不改变配置参数。 */
void PaperCounter_Reset(void);

/* 输入一个频率样本；稳定后输出标定换算的纸张数量。 */
PaperCounterStatus PaperCounter_ProcessFrequency(uint32_t frequency_hz, uint16_t *paper_count);

#endif
