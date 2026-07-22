#include "stm32f10x.h"
#include "delay.h"
#include "bsp_led.h"

int main(void)
{
    Delay_Init();
    LED_Init();

    while (1)
    {
        LED_Toggle();
        Delay_ms(500);
    }
}


