#include "stm32f10x.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_usart.h"
#include "bsp_oled.h"
#include "bsp_freq.h"
#include "bsp_at24c08.h"
#include "bsp_buzzer.h"
#include "app_calibration.h"
#include "app_paper_counter.h"
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
    else if (state->page == UI_PAGE_CALIBRATION)
    {
        OLED_ShowNumber(42U, 0U, state->calibration_paper_count);
        OLED_ShowNumber(42U, 2U, state->calibration_frequency_hz);
    }
}

static void Main_HandleCounterResult(PaperCounterStatus status, uint16_t paper_count,
                                     PaperCounterStatus *previous_status, uint16_t *previous_count)
{
    if ((status == PAPER_COUNTER_STATUS_OK) &&
        ((*previous_status != PAPER_COUNTER_STATUS_OK) || (*previous_count != paper_count)))
    {
        Buzzer_Beep(120U);
    }
    else if (((status == PAPER_COUNTER_STATUS_FREQUENCY_LOW) ||
              (status == PAPER_COUNTER_STATUS_FREQUENCY_HIGH)) &&
             (*previous_status != status))
    {
        /* 越界使用一次较长提示音，避免阻塞主循环。 */
        Buzzer_Beep(240U);
    }

    *previous_status = status;
    *previous_count = paper_count;
}

int main(void)
{
    uint8_t led_tick = 0U;
    uint8_t usart_tick = 0U;
    uint8_t oled_ok;
    CalibrationStorageStatus calibration_storage_status;
    UiAction ui_action;
    CalibrationStatus calibration_status;
    PaperCounterConfig paper_counter_config;
    PaperCounterStatus counter_status;
    PaperCounterStatus previous_counter_status;
    uint32_t frequency_hz;
    uint16_t paper_count = 0U;
    uint16_t previous_paper_count;
    KeyCode key;
    UiPage previous_page;

    Delay_Init();
    LED_Init();
    Key_Init();
    USART1_Init(115200U);
    /* 等待 USB 转串口模块与上位机在复位后稳定，避免首批调试字符丢失。 */
    Delay_ms(1000U);
    Freq_Init();
    Buzzer_Init();
    Buzzer_Beep(150U);
    paper_counter_config.frequency_tolerance_hz = 300U;
    paper_counter_config.required_stable_samples = 3U;
    PaperCounter_Init(&paper_counter_config);
    previous_counter_status = PAPER_COUNTER_STATUS_WAITING_FOR_STABLE;
    previous_paper_count = 0U;
    Ui_Init();
    AT24C08_Init();

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

    /* 开机仅加载已有标定表，不自动写入 EEPROM。 */
    calibration_storage_status = Calibration_Load();
    printf("Calibration load status: %d\r\n", (int)calibration_storage_status);

    printf("\r\nPaperCounter boot OK\r\n");

    while (1)
    {
        key = Key_Scan();
        if (key != KEY_NONE)
        {
            printf("KEY %d pressed\r\n", (int)key);
            previous_page = Ui_GetState()->page;
            ui_action = Ui_HandleKey(Main_ConvertKeyEvent(key));
            if (ui_action == UI_ACTION_CANCEL_EDIT)
            {
                Calibration_DiscardEdit();
                Main_RenderUi();
            }
            else if (ui_action == UI_ACTION_CAPTURE_REQUEST)
            {
                calibration_status = Calibration_SetEditPoint(Ui_GetState()->calibration_paper_count,
                                                              Ui_GetState()->calibration_frequency_hz);
                printf("Calibration capture status: %d\r\n", (int)calibration_status);
                Ui_CompleteCapture(calibration_status == CALIBRATION_STATUS_OK);
                if (calibration_status == CALIBRATION_STATUS_OK)
                {
                    Buzzer_Beep(100U);
                }
                Main_RenderUi();
            }
            else if (ui_action == UI_ACTION_SAVE_REQUEST)
            {
                calibration_storage_status = Calibration_SaveEdit();
                printf("Calibration save status: %d\r\n", (int)calibration_storage_status);
                Ui_CompleteSave(calibration_storage_status == CALIBRATION_STORAGE_STATUS_OK);
                if (calibration_storage_status == CALIBRATION_STORAGE_STATUS_OK)
                {
                    Buzzer_Beep(200U);
                }
                else
                {
                    /* 保存失败使用一次较长提示音，避免阻塞主循环。 */
                    Buzzer_Beep(240U);
                }
                Main_RenderUi();
            }
            else if (ui_action == UI_ACTION_RENDER)
            {
                if ((previous_page == UI_PAGE_MONITOR) && (Ui_GetState()->page == UI_PAGE_CALIBRATION))
                {
                    Calibration_BeginEdit();
                }
                Main_RenderUi();
            }
            else
            {
                printf("UI action ignored\r\n");
            }
        }

        Delay_ms(10U);
        Buzzer_Task(10U);

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
                Ui_UpdateCalibrationFrequency(frequency_hz);
                if (Ui_GetState()->page == UI_PAGE_MONITOR)
                {
                    counter_status = PaperCounter_ProcessFrequency(frequency_hz, &paper_count);
                    Ui_UpdateMeasurement(frequency_hz, paper_count, counter_status);
                    Main_HandleCounterResult(counter_status, paper_count,
                                             &previous_counter_status, &previous_paper_count);
                    Main_RenderUi();
                }
            }
        }
    }
}
