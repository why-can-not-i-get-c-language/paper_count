#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#include "stm32f10x.h"

/* 四个按键均使用内部上拉，按下时连接到 GND。 */
#define KEY_GPIO_PORT GPIOB
#define KEY1_PIN       GPIO_Pin_12
#define KEY2_PIN       GPIO_Pin_13
#define KEY3_PIN       GPIO_Pin_14
#define KEY4_PIN       GPIO_Pin_15
#define KEY_ALL_PINS   (KEY1_PIN | KEY2_PIN | KEY3_PIN | KEY4_PIN)

/* 软件 I2C 使用 PB6 作为时钟、PB7 作为数据线。 */
#define SOFT_I2C_GPIO_PORT GPIOB
#define SOFT_I2C_SCL_PIN   GPIO_Pin_6
#define SOFT_I2C_SDA_PIN   GPIO_Pin_7

#define OLED_I2C_ADDRESS   0x3CU

/* TIM2_CH1 使用 PA0 输入 NE555 或自测方波。 */
#define FREQ_GPIO_PORT GPIOA
#define FREQ_INPUT_PIN GPIO_Pin_0

#endif
