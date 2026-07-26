#include "bsp_freq.h"
#include "board_config.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"

#define FREQ_TIMER_CLOCK_HZ 1000000UL

static volatile uint32_t timer_overflow_count;
static volatile uint32_t last_capture_timestamp;
static volatile uint32_t captured_period_ticks;
static volatile uint8_t capture_ready;
static volatile uint8_t first_capture_received;

void Freq_Init(void)
{
    GPIO_InitTypeDef gpio_init;
    TIM_TimeBaseInitTypeDef timer_init;
    TIM_ICInitTypeDef capture_init;
    NVIC_InitTypeDef nvic_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    gpio_init.GPIO_Pin = FREQ_INPUT_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(FREQ_GPIO_PORT, &gpio_init);

    timer_init.TIM_Period = 0xFFFFU;
    timer_init.TIM_Prescaler = 71U;
    timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
    timer_init.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &timer_init);

    capture_init.TIM_Channel = TIM_Channel_1;
    capture_init.TIM_ICPolarity = TIM_ICPolarity_Rising;
    capture_init.TIM_ICSelection = TIM_ICSelection_DirectTI;
    capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    capture_init.TIM_ICFilter = 0x00U;
    TIM_ICInit(TIM2, &capture_init);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update | TIM_IT_CC1);
    TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1, ENABLE);

    nvic_init.NVIC_IRQChannel = TIM2_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 1U;
    nvic_init.NVIC_IRQChannelSubPriority = 0U;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);

    timer_overflow_count = 0U;
    capture_ready = 0U;
    first_capture_received = 0U;
    TIM_Cmd(TIM2, ENABLE);
}

uint8_t Freq_GetHz(uint32_t *frequency_hz)
{
    uint32_t period_ticks;

    if ((frequency_hz == 0) || (capture_ready == 0U))
    {
        return 0U;
    }

    period_ticks = captured_period_ticks;
    if (period_ticks == 0U)
    {
        return 0U;
    }

    *frequency_hz = FREQ_TIMER_CLOCK_HZ / period_ticks;
    return 1U;
}

void Freq_TIM2IRQHandler(void)
{
    uint16_t status;
    uint16_t capture_value;
    uint32_t capture_timestamp;

    status = TIM2->SR;
    if ((status & TIM_SR_UIF) != 0U)
    {
        timer_overflow_count++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }

    if ((status & TIM_SR_CC1IF) != 0U)
    {
        capture_value = TIM_GetCapture1(TIM2);
        capture_timestamp = (timer_overflow_count << 16U) | capture_value;

        /* 捕获值较大且同时溢出时，边沿发生在溢出之前。 */
        if (((status & TIM_SR_UIF) != 0U) && (capture_value > 0x8000U))
        {
            capture_timestamp -= 0x10000UL;
        }

        if (first_capture_received != 0U)
        {
            captured_period_ticks = capture_timestamp - last_capture_timestamp;
            capture_ready = 1U;
        }

        last_capture_timestamp = capture_timestamp;
        first_capture_received = 1U;
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    }
}
