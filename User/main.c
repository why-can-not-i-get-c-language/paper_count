#include "stm32f10x.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_usart.h"
#include "bsp_oled.h"
#include "bsp_freq.h"
#include <stdio.h>

int main(void)
{
    uint8_t led_tick = 0U;
    uint8_t usart_tick = 0U;
    uint8_t oled_ok;
    uint32_t frequency_hz;
    KeyCode key;

    Delay_Init();
    LED_Init();
    Key_Init();
    USART1_Init(115200U);
    Freq_Init();

    oled_ok = OLED_Init();
    if (oled_ok != 0U)
    {
        OLED_ShowString(0U, 0U, "OLED OK");
        printf("OLED init OK\r\n");
    }
    else
    {
        printf("OLED init failed\r\n");
    }

    printf("\r\nPaperCounter boot OK\r\n");

    while (1)
    {
        key = Key_Scan();
        if (key != KEY_NONE)
        {
            printf("KEY %d pressed\r\n", (int)key);
        }

        Delay_ms(10U);

        led_tick++;
        if (led_tick >= 50U)
        {
            led_tick = 0U;
            LED_Toggle();
        }

        usart_tick++;
        if (usart_tick >= 100U)
        {
            usart_tick = 0U;
            printf("System is running.\r\n");
            if (Freq_GetHz(&frequency_hz) != 0U)
            {
                printf("Frequency: %u Hz\r\n", (unsigned int)frequency_hz);
                OLED_ShowNumber(0U, 2U, frequency_hz);
            }
        }
    }
}
