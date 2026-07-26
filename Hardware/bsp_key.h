#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f10x.h"

typedef enum
{
    KEY_NONE = 0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4
} KeyCode;

void Key_Init(void);
KeyCode Key_Scan(void);

#endif
