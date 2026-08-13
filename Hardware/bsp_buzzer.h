#ifndef __BSP_BUZZER_H
#define __BSP_BUZZER_H

#include "stm32f10x.h"

/* 初始化 PB1/TIM3_CH4 蜂鸣器输出，默认关闭。 */
void Buzzer_Init(void);

/* 开始输出约 2048Hz 方波。 */
void Buzzer_Start(void);

/* 立即关闭蜂鸣器输出。 */
void Buzzer_Stop(void);

/* 请求一次非阻塞短鸣，持续时间单位为毫秒。 */
void Buzzer_Beep(uint16_t duration_ms);

/* 每经过一段已知时间调用一次，用于结束短鸣。 */
void Buzzer_Task(uint16_t elapsed_ms);

#endif
