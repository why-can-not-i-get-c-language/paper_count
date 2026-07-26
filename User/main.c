#include "stm32f10x.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_usart.h"
#include "bsp_oled.h"
#include "bsp_freq.h"
#include "app_ui.h"
#include <stdio.h>

static UiKeyEvent Main_ConvertKeyEvent(KeyCode key)
{
    if (key == KEY_1) return UI_KEY_1;
    if (key == KEY_2) return UI_KEY_2;
    if (key == KEY_3) return UI_KEY_3;
    if (key == KEY_4) return UI_KEY_4;
    return UI_KEY_NONE;
}

static void Main_RenderUi(void)
{
    const UiState *state;

    state = Ui_GetState();
    OLED_Clear();
    OLED_ShowString(0U, 0U, Ui_GetTitle());
    OLED_ShowString(0U, 2U, Ui_GetDetail());
    if (state->page == UI_PAGE_MONITOR)
    {
        OLED_ShowNumber(42U, 0U, state->frequency_hz);
        OLED_ShowNumber(42U, 2U, state->paper_count);
    }
}

int main(void)
{
    uint8_t led_tick = 0U;
    uint8_t usart_tick = 0U;
    uint8_t oled_ok;
    uint32_t frequency_hz;
    KeyCode key;

    Delay_Init();
    LED_Init();
    Key_Init();
    USART1_Init(115200U);
    Freq_Init();
    Ui_Init();

    oled_ok = OLED_Init();
    if (oled_ok != 0U)
    {
        OLED_ShowString(0U, 0U, "OLED OK");
        printf("OLED init OK\r\n");
        Main_RenderUi();
    }
    else
    {
        printf("OLED init failed\r\n");
    }

    printf("\r\nPaperCounter boot OK\r\n");

    while (1)
    {
        key = Key_Scan();
        if (key != KEY_NONE)
        {
            printf("KEY %d pressed\r\n", (int)key);
            if (Ui_HandleKey(Main_ConvertKeyEvent(key)) != 0U)
            {
                Main_RenderUi();
            }
            else
            {
                printf("UI action ignored\r\n");
            }
        }

        Delay_ms(10U);

        led_tick++;
        if (led_tick >= 50U)
        {
            led_tick = 0U;
            LED_Toggle();
        }

        usart_tick++;
        if (usart_tick >= 100U)
        {
            usart_tick = 0U;
            printf("System is running.\r\n");
            if (Freq_GetHz(&frequency_hz) != 0U)
            {
                printf("Frequency: %u Hz\r\n", (unsigned int)frequency_hz);
                if (Ui_GetState()->page == UI_PAGE_MONITOR)
                {
                    Ui_UpdateMeasurement(frequency_hz, 0U, PAPER_COUNTER_STATUS_WAITING_FOR_STABLE);
                    Main_RenderUi();
                }
            }
        }
    }
}
