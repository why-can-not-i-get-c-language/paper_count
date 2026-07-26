#ifndef __APP_UI_H
#define __APP_UI_H

#include <stdint.h>
#include "app_paper_counter.h"

typedef enum
{
    UI_PAGE_MONITOR = 0,
    UI_PAGE_CALIBRATION,
    UI_PAGE_SAVE_CONFIRM
} UiPage;

typedef enum
{
    UI_KEY_NONE = 0,
    UI_KEY_1,
    UI_KEY_2,
    UI_KEY_3,
    UI_KEY_4
} UiKeyEvent;

typedef enum
{
    UI_ACTION_NONE = 0,
    UI_ACTION_RENDER,
    UI_ACTION_SAVE_REQUEST
} UiAction;

typedef struct
{
    UiPage page;
    uint32_t frequency_hz;
    uint16_t paper_count;
    PaperCounterStatus counter_status;
    uint8_t calibration_dirty;
    uint8_t save_failed;
} UiState;

/* 初始化页面状态。 */
void Ui_Init(void);

/* 处理已转换的按键事件，并返回界面或保存动作。 */
UiAction Ui_HandleKey(UiKeyEvent key_event);

/* 根据 EEPROM 保存结果完成或保留保存确认页。 */
uint8_t Ui_CompleteSave(uint8_t success);

/* 更新监测页显示所需的测量数据。 */
void Ui_UpdateMeasurement(uint32_t frequency_hz, uint16_t paper_count, PaperCounterStatus counter_status);

/* 获取当前页面与显示状态，供 OLED 或串口显示模块读取。 */
const UiState *Ui_GetState(void);
const char *Ui_GetTitle(void);
const char *Ui_GetDetail(void);

#endif
