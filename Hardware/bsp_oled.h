#ifndef __BSP_OLED_H
#define __BSP_OLED_H

#include "stm32f10x.h"

uint8_t OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t column, uint8_t page, char character);
void OLED_ShowString(uint8_t column, uint8_t page, const char *text);
void OLED_ShowNumber(uint8_t column, uint8_t page, uint32_t number);

#endif
