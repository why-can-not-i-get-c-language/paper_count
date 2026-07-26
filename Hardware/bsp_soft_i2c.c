#include "bsp_soft_i2c.h"
#include "board_config.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define SOFT_I2C_ACK_TIMEOUT 1000U

static void SoftI2C_Delay(void)
{
    volatile uint8_t index;

    for (index = 0U; index < 10U; index++)
    {
        __NOP();
    }
}

static void SoftI2C_SetSCL(uint8_t level)
{
    if (level != 0U)
    {
        GPIO_SetBits(SOFT_I2C_GPIO_PORT, SOFT_I2C_SCL_PIN);
    }
    else
    {
        GPIO_ResetBits(SOFT_I2C_GPIO_PORT, SOFT_I2C_SCL_PIN);
    }
}

static void SoftI2C_SetSDA(uint8_t level)
{
    if (level != 0U)
    {
        GPIO_SetBits(SOFT_I2C_GPIO_PORT, SOFT_I2C_SDA_PIN);
    }
    else
    {
        GPIO_ResetBits(SOFT_I2C_GPIO_PORT, SOFT_I2C_SDA_PIN);
    }
}

void SoftI2C_Init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpio_init.GPIO_Pin = SOFT_I2C_SCL_PIN | SOFT_I2C_SDA_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(SOFT_I2C_GPIO_PORT, &gpio_init);
    SoftI2C_SetSCL(1U);
    SoftI2C_SetSDA(1U);
}

void SoftI2C_Start(void)
{
    SoftI2C_SetSDA(1U);
    SoftI2C_SetSCL(1U);
    SoftI2C_Delay();
    SoftI2C_SetSDA(0U);
    SoftI2C_Delay();
    SoftI2C_SetSCL(0U);
}

void SoftI2C_Stop(void)
{
    SoftI2C_SetSDA(0U);
    SoftI2C_Delay();
    SoftI2C_SetSCL(1U);
    SoftI2C_Delay();
    SoftI2C_SetSDA(1U);
    SoftI2C_Delay();
}

uint8_t SoftI2C_SendByte(uint8_t data)
{
    uint8_t bit_index;
    uint16_t timeout;

    for (bit_index = 0U; bit_index < 8U; bit_index++)
    {
        SoftI2C_SetSDA((data & 0x80U) != 0U);
        SoftI2C_Delay();
        SoftI2C_SetSCL(1U);
        SoftI2C_Delay();
        SoftI2C_SetSCL(0U);
        data <<= 1U;
    }

    SoftI2C_SetSDA(1U);
    SoftI2C_Delay();
    SoftI2C_SetSCL(1U);
    timeout = SOFT_I2C_ACK_TIMEOUT;
    while ((GPIO_ReadInputDataBit(SOFT_I2C_GPIO_PORT, SOFT_I2C_SDA_PIN) != Bit_RESET) && (timeout > 0U))
    {
        timeout--;
    }
    SoftI2C_SetSCL(0U);

    return (timeout > 0U) ? 1U : 0U;
}

uint8_t SoftI2C_ReadByte(uint8_t send_ack)
{
    uint8_t bit_index;
    uint8_t data;

    data = 0U;
    SoftI2C_SetSDA(1U);
    for (bit_index = 0U; bit_index < 8U; bit_index++)
    {
        data <<= 1U;
        SoftI2C_SetSCL(1U);
        SoftI2C_Delay();
        if (GPIO_ReadInputDataBit(SOFT_I2C_GPIO_PORT, SOFT_I2C_SDA_PIN) != Bit_RESET)
        {
            data |= 0x01U;
        }
        SoftI2C_SetSCL(0U);
        SoftI2C_Delay();
    }

    SoftI2C_SetSDA((send_ack != 0U) ? 0U : 1U);
    SoftI2C_Delay();
    SoftI2C_SetSCL(1U);
    SoftI2C_Delay();
    SoftI2C_SetSCL(0U);
    SoftI2C_SetSDA(1U);

    return data;
}
