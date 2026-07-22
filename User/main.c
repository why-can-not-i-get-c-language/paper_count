#include "stm32f10x.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include <stdio.h>

int main(void)
{
    uint8_t heartbeat = 0U;

    Delay_Init();
    LED_Init();
    USART1_Init(115200U);

    printf("\r\nPaperCounter boot OK\r\n");

    while (1)
    {
        LED_Toggle();

        if (heartbeat == 0U)
        {
            printf("System is running.\r\n");
        }

        heartbeat = 1U - heartbeat;
        Delay_ms(500);
    }
}
