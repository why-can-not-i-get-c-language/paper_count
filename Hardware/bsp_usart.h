#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f10x.h"

void USART1_Init(uint32_t baudrate);
void USART1_SendByte(uint8_t data);
void USART1_SendString(const char *text);

#endif
