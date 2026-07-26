#include "app_ui.h"

static UiState ui_state;

void Ui_Init(void)
{
    ui_state.page = UI_PAGE_MONITOR;
    ui_state.frequency_hz = 0U;
    ui_state.paper_count = 0U;
    ui_state.counter_status = PAPER_COUNTER_STATUS_WAITING_FOR_STABLE;
    ui_state.calibration_dirty = 0U;
    ui_state.save_failed = 0U;
}

UiAction Ui_HandleKey(UiKeyEvent key_event)
{
    if (ui_state.page == UI_PAGE_MONITOR)
    {
        if (key_event == UI_KEY_2)
        {
            ui_state.page = UI_PAGE_CALIBRATION;
            ui_state.calibration_dirty = 0U;
            ui_state.save_failed = 0U;
            return UI_ACTION_RENDER;
        }
    }
    else if (ui_state.page == UI_PAGE_CALIBRATION)
    {
        if (key_event == UI_KEY_1)
        {
            ui_state.page = UI_PAGE_MONITOR;
            ui_state.calibration_dirty = 0U;
            ui_state.save_failed = 0U;
            return UI_ACTION_RENDER;
        }
        if (key_event == UI_KEY_3)
        {
            ui_state.calibration_dirty = 1U;
            return UI_ACTION_RENDER;
        }
        if (key_event == UI_KEY_4)
        {
            ui_state.page = UI_PAGE_SAVE_CONFIRM;
            ui_state.save_failed = 0U;
            return UI_ACTION_RENDER;
        }
    }
    else if (ui_state.page == UI_PAGE_SAVE_CONFIRM)
    {
        if ((key_event == UI_KEY_1) || (key_event == UI_KEY_2))
        {
            ui_state.page = UI_PAGE_CALIBRATION;
            ui_state.save_failed = 0U;
            return UI_ACTION_RENDER;
        }
        if (key_event == UI_KEY_4)
        {
            return UI_ACTION_SAVE_REQUEST;
        }
    }

    return UI_ACTION_NONE;
}

uint8_t Ui_CompleteSave(uint8_t success)
{
    if (ui_state.page != UI_PAGE_SAVE_CONFIRM)
    {
        return 0U;
    }

    if (success == 0U)
    {
        ui_state.save_failed = 1U;
        return 1U;
    }

    ui_state.calibration_dirty = 0U;
    ui_state.save_failed = 0U;
    ui_state.page = UI_PAGE_MONITOR;
    return 1U;
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

const char *Ui_GetTitle(void)
{
    if (ui_state.page == UI_PAGE_CALIBRATION)
    {
        return "CAL MODE";
    }

    if (ui_state.page == UI_PAGE_SAVE_CONFIRM)
    {
        return "SAVE CAL?";
    }

    return "FREQ:";
}

const char *Ui_GetDetail(void)
{
    if (ui_state.page == UI_PAGE_CALIBRATION)
    {
        return (ui_state.calibration_dirty != 0U) ? "DIRTY" : "EDIT";
    }

    if (ui_state.page == UI_PAGE_SAVE_CONFIRM)
    {
        if (ui_state.save_failed != 0U)
        {
            return "SAVE FAIL";
        }
        return "K4: YES";
    }

    return "COUNT:";
}
