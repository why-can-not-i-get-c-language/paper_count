#ifndef __BSP_FREQ_H
#define __BSP_FREQ_H

#include "stm32f10x.h"

void Freq_Init(void);
uint8_t Freq_GetHz(uint32_t *frequency_hz);
void Freq_TIM2IRQHandler(void);

#endif
