#include "bsp_buzzer.h"
#include "board_config.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#define BUZZER_PWM_PERIOD     488U
#define BUZZER_PWM_PULSE      244U

static uint16_t buzzer_remaining_ms;

void Buzzer_Init(void)
{
    GPIO_InitTypeDef gpio_init;
    TIM_TimeBaseInitTypeDef timer_init;
    TIM_OCInitTypeDef output_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    gpio_init.GPIO_Pin = BUZZER_GPIO_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(BUZZER_GPIO_PORT, &gpio_init);

    timer_init.TIM_Period = BUZZER_PWM_PERIOD - 1U;
    timer_init.TIM_Prescaler = 71U;
    timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
    timer_init.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &timer_init);

    output_init.TIM_OCMode = TIM_OCMode_PWM1;
    output_init.TIM_OutputState = TIM_OutputState_Disable;
    output_init.TIM_Pulse = BUZZER_PWM_PULSE;
    output_init.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init(TIM3, &output_init);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_SetCompare4(TIM3, BUZZER_PWM_PULSE);
    TIM_Cmd(TIM3, ENABLE);

    buzzer_remaining_ms = 0U;
    Buzzer_Stop();
}

void Buzzer_Start(void)
{
    TIM_SetCompare4(TIM3, BUZZER_PWM_PULSE);
    TIM_CCxCmd(TIM3, TIM_Channel_4, TIM_CCx_Enable);
}

void Buzzer_Stop(void)
{
    TIM_CCxCmd(TIM3, TIM_Channel_4, TIM_CCx_Disable);
    buzzer_remaining_ms = 0U;
    GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}

void Buzzer_Beep(uint16_t duration_ms)
{
    if (duration_ms == 0U)
    {
        Buzzer_Stop();
        return;
    }

    buzzer_remaining_ms = duration_ms;
    Buzzer_Start();
}

void Buzzer_Task(uint16_t elapsed_ms)
{
    if (buzzer_remaining_ms == 0U)
    {
        return;
    }

    if (elapsed_ms >= buzzer_remaining_ms)
    {
        Buzzer_Stop();
    }
    else
    {
        buzzer_remaining_ms = (uint16_t)(buzzer_remaining_ms - elapsed_ms);
    }
}
