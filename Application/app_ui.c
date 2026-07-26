#include "app_ui.h"

static UiState ui_state;

void Ui_Init(void)
{
    ui_state.page = UI_PAGE_MONITOR;
    ui_state.frequency_hz = 0U;
    ui_state.paper_count = 0U;
    ui_state.counter_status = PAPER_COUNTER_STATUS_WAITING_FOR_STABLE;
    ui_state.calibration_dirty = 0U;
}

void Ui_HandleKey(UiKeyEvent key_event)
{
    if (key_event == UI_KEY_1)
    {
        ui_state.page = UI_PAGE_MONITOR;
    }
    else if (key_event == UI_KEY_2)
    {
        ui_state.page = UI_PAGE_CALIBRATION;
    }
    else if (key_event == UI_KEY_3)
    {
        if (ui_state.page == UI_PAGE_CALIBRATION)
        {
            ui_state.calibration_dirty = 1U;
        }
    }
    else if (key_event == UI_KEY_4)
    {
        if (ui_state.page == UI_PAGE_CALIBRATION)
        {
            ui_state.page = UI_PAGE_SAVE_CONFIRM;
        }
        else if (ui_state.page == UI_PAGE_SAVE_CONFIRM)
        {
            ui_state.calibration_dirty = 0U;
            ui_state.page = UI_PAGE_MONITOR;
        }
    }
}

void Ui_UpdateMeasurement(uint32_t frequency_hz, uint16_t paper_count, PaperCounterStatus counter_status)
{
    ui_state.frequency_hz = frequency_hz;
    ui_state.paper_count = paper_count;
    ui_state.counter_status = counter_status;
}

const UiState *Ui_GetState(void)
{
    return &ui_state;
}
