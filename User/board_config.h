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

#endif
