#include "bsp_led.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define LED_PORT GPIOC
#define LED_PIN  GPIO_Pin_13

void LED_Init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    gpio_init.GPIO_Pin = LED_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_PORT, &gpio_init);

    LED_Off();
}

void LED_On(void)
{
    GPIO_ResetBits(LED_PORT, LED_PIN);
}

void LED_Off(void)
{
    GPIO_SetBits(LED_PORT, LED_PIN);
}

void LED_Toggle(void)
{
    GPIO_WriteBit(LED_PORT, LED_PIN,
                  (BitAction)(1U - GPIO_ReadOutputDataBit(LED_PORT, LED_PIN)));
}
