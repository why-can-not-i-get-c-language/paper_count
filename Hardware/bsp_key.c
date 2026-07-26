#include "bsp_key.h"
#include "board_config.h"
#include "delay.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define KEY_DEBOUNCE_MS 20U

static void Key_WaitRelease(uint16_t pin)
{
    while (GPIO_ReadInputDataBit(KEY_GPIO_PORT, pin) == Bit_RESET)
    {
        Delay_ms(10U);
    }
}

void Key_Init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    gpio_init.GPIO_Pin = KEY_ALL_PINS;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY_GPIO_PORT, &gpio_init);

    /* 输入上拉模式需要将对应端口输出位置为高电平。 */
    GPIO_SetBits(KEY_GPIO_PORT, KEY_ALL_PINS);
}

KeyCode Key_Scan(void)
{
    if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY1_PIN) == Bit_RESET)
    {
        Delay_ms(KEY_DEBOUNCE_MS);
        if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY1_PIN) == Bit_RESET)
        {
            Key_WaitRelease(KEY1_PIN);
            return KEY_1;
        }
    }

    if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY2_PIN) == Bit_RESET)
    {
        Delay_ms(KEY_DEBOUNCE_MS);
        if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY2_PIN) == Bit_RESET)
        {
            Key_WaitRelease(KEY2_PIN);
            return KEY_2;
        }
    }

    if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY3_PIN) == Bit_RESET)
    {
        Delay_ms(KEY_DEBOUNCE_MS);
        if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY3_PIN) == Bit_RESET)
        {
            Key_WaitRelease(KEY3_PIN);
            return KEY_3;
        }
    }

    if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY4_PIN) == Bit_RESET)
    {
        Delay_ms(KEY_DEBOUNCE_MS);
        if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY4_PIN) == Bit_RESET)
        {
            Key_WaitRelease(KEY4_PIN);
            return KEY_4;
        }
    }

    return KEY_NONE;
}
