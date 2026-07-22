#include "bsp_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include <stdio.h>

#define USART1_TX_TIMEOUT 100000U

/* 禁用半主机输出，避免 printf 在未连接调试器时阻塞。 */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm(".global __use_no_semihosting");
#else
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};
#endif

FILE __stdout;

void _sys_exit(int x)
{
    (void)x;
}

void USART1_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef gpio_init;
    USART_InitTypeDef usart_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    /* PA9：USART1 发送引脚。 */
    gpio_init.GPIO_Pin = GPIO_Pin_9;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio_init);

    /* PA10：USART1 接收引脚，预留给后续串口输入功能。 */
    gpio_init.GPIO_Pin = GPIO_Pin_10;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_init);

    USART_StructInit(&usart_init);
    usart_init.USART_BaudRate = baudrate;
    usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &usart_init);
    USART_Cmd(USART1, ENABLE);
}

void USART1_SendByte(uint8_t data)
{
    uint32_t timeout = USART1_TX_TIMEOUT;

    /* 发送前等待发送数据寄存器为空，防止异常状态永久阻塞主循环。 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        if (timeout == 0U)
        {
            return;
        }

        timeout--;
    }

    USART_SendData(USART1, data);
}

void USART1_SendString(const char *text)
{
    while (*text != '\0')
    {
        USART1_SendByte((uint8_t)*text);
        text++;
    }
}

int fputc(int ch, FILE *stream)
{
    (void)stream;
    USART1_SendByte((uint8_t)ch);
    return ch;
}
