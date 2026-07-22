#include "delay.h"

static volatile uint32_t s_ms_ticks;

void Delay_Init(void)
{
    SysTick_Config(SystemCoreClock / 1000U);
}

void Delay_SysTickHandler(void)
{
    s_ms_ticks++;
}

void Delay_ms(uint32_t ms)
{
    uint32_t start = s_ms_ticks;

    while ((uint32_t)(s_ms_ticks - start) < ms)
    {
    }
}
