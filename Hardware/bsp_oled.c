#include "bsp_oled.h"
#include "bsp_soft_i2c.h"
#include "board_config.h"

static uint8_t OLED_SendCommand(uint8_t command)
{
    SoftI2C_Start();
    if (SoftI2C_SendByte((uint8_t)(OLED_I2C_ADDRESS << 1U)) == 0U)
    {
        SoftI2C_Stop();
        return 0U;
    }
    SoftI2C_SendByte(0x00U);
    SoftI2C_SendByte(command);
    SoftI2C_Stop();
    return 1U;
}

static void OLED_SetPosition(uint8_t column, uint8_t page)
{
    OLED_SendCommand((uint8_t)(0xB0U | page));
    OLED_SendCommand((uint8_t)(0x00U | (column & 0x0FU)));
    OLED_SendCommand((uint8_t)(0x10U | ((column >> 4U) & 0x0FU)));
}

static void OLED_SendData(uint8_t data)
{
    SoftI2C_Start();
    SoftI2C_SendByte((uint8_t)(OLED_I2C_ADDRESS << 1U));
    SoftI2C_SendByte(0x40U);
    SoftI2C_SendByte(data);
    SoftI2C_Stop();
}

static void OLED_GetFont(char character, const uint8_t **font)
{
    static const uint8_t blank[5] = {0x00U, 0x00U, 0x00U, 0x00U, 0x00U};
    static const uint8_t digit[10][5] = {
        {0x3EU, 0x51U, 0x49U, 0x45U, 0x3EU}, {0x00U, 0x42U, 0x7FU, 0x40U, 0x00U},
        {0x42U, 0x61U, 0x51U, 0x49U, 0x46U}, {0x21U, 0x41U, 0x45U, 0x4BU, 0x31U},
        {0x18U, 0x14U, 0x12U, 0x7FU, 0x10U}, {0x27U, 0x45U, 0x45U, 0x45U, 0x39U},
        {0x3CU, 0x4AU, 0x49U, 0x49U, 0x30U}, {0x01U, 0x71U, 0x09U, 0x05U, 0x03U},
        {0x36U, 0x49U, 0x49U, 0x49U, 0x36U}, {0x06U, 0x49U, 0x49U, 0x29U, 0x1EU}
    };
    *font = blank;
    if ((character >= '0') && (character <= '9'))
    {
        *font = digit[(uint8_t)(character - '0')];
    }
    else if (character == 'A') { static const uint8_t data[5] = {0x7EU,0x11U,0x11U,0x11U,0x7EU}; *font = data; }
    else if (character == 'C') { static const uint8_t data[5] = {0x3EU,0x41U,0x41U,0x41U,0x22U}; *font = data; }
    else if (character == 'D') { static const uint8_t data[5] = {0x7FU,0x41U,0x41U,0x22U,0x1CU}; *font = data; }
    else if (character == 'E') { static const uint8_t data[5] = {0x7FU,0x49U,0x49U,0x49U,0x41U}; *font = data; }
    else if (character == 'F') { static const uint8_t data[5] = {0x7FU,0x09U,0x09U,0x09U,0x01U}; *font = data; }
    else if (character == 'I') { static const uint8_t data[5] = {0x00U,0x41U,0x7FU,0x41U,0x00U}; *font = data; }
    else if (character == 'K') { static const uint8_t data[5] = {0x7FU,0x08U,0x14U,0x22U,0x41U}; *font = data; }
    else if (character == 'L') { static const uint8_t data[5] = {0x7FU,0x40U,0x40U,0x40U,0x40U}; *font = data; }
    else if (character == 'M') { static const uint8_t data[5] = {0x7FU,0x02U,0x0CU,0x02U,0x7FU}; *font = data; }
    else if (character == 'N') { static const uint8_t data[5] = {0x7FU,0x04U,0x08U,0x10U,0x7FU}; *font = data; }
    else if (character == 'O') { static const uint8_t data[5] = {0x3EU,0x41U,0x41U,0x41U,0x3EU}; *font = data; }
    else if (character == 'Q') { static const uint8_t data[5] = {0x3EU,0x41U,0x51U,0x21U,0x5EU}; *font = data; }
    else if (character == 'R') { static const uint8_t data[5] = {0x7FU,0x09U,0x19U,0x29U,0x46U}; *font = data; }
    else if (character == 'S') { static const uint8_t data[5] = {0x46U,0x49U,0x49U,0x49U,0x31U}; *font = data; }
    else if (character == 'T') { static const uint8_t data[5] = {0x01U,0x01U,0x7FU,0x01U,0x01U}; *font = data; }
    else if (character == 'U') { static const uint8_t data[5] = {0x3FU,0x40U,0x40U,0x40U,0x3FU}; *font = data; }
    else if (character == 'V') { static const uint8_t data[5] = {0x1FU,0x20U,0x40U,0x20U,0x1FU}; *font = data; }
    else if (character == 'Y') { static const uint8_t data[5] = {0x07U,0x08U,0x70U,0x08U,0x07U}; *font = data; }
    else if (character == ':') { static const uint8_t data[5] = {0x00U,0x36U,0x36U,0x00U,0x00U}; *font = data; }
}

uint8_t OLED_Init(void)
{
    SoftI2C_Init();
    if (OLED_SendCommand(0xAEU) == 0U)
    {
        return 0U;
    }
    OLED_SendCommand(0xD5U); OLED_SendCommand(0x80U);
    OLED_SendCommand(0xA8U); OLED_SendCommand(0x3FU);
    OLED_SendCommand(0xD3U); OLED_SendCommand(0x00U);
    OLED_SendCommand(0x40U); OLED_SendCommand(0x8DU); OLED_SendCommand(0x14U);
    OLED_SendCommand(0x20U); OLED_SendCommand(0x00U);
    OLED_SendCommand(0xA1U); OLED_SendCommand(0xC8U);
    OLED_SendCommand(0xDAU); OLED_SendCommand(0x12U);
    OLED_SendCommand(0x81U); OLED_SendCommand(0xCFU);
    OLED_SendCommand(0xD9U); OLED_SendCommand(0xF1U);
    OLED_SendCommand(0xDBU); OLED_SendCommand(0x40U);
    OLED_SendCommand(0xA4U); OLED_SendCommand(0xA6U); OLED_SendCommand(0xAFU);
    OLED_Clear();
    return 1U;
}

void OLED_Clear(void)
{
    uint8_t page;
    uint8_t column;

    for (page = 0U; page < 8U; page++)
    {
        OLED_SetPosition(0U, page);
        for (column = 0U; column < 128U; column++)
        {
            OLED_SendData(0x00U);
        }
    }
}

void OLED_ShowChar(uint8_t column, uint8_t page, char character)
{
    const uint8_t *font;
    uint8_t index;

    OLED_GetFont(character, &font);
    OLED_SetPosition(column, page);
    for (index = 0U; index < 5U; index++)
    {
        OLED_SendData(font[index]);
    }
    OLED_SendData(0x00U);
}

void OLED_ShowString(uint8_t column, uint8_t page, const char *text)
{
    while ((*text != '\0') && (column <= 122U))
    {
        OLED_ShowChar(column, page, *text);
        column = (uint8_t)(column + 6U);
        text++;
    }
}

void OLED_ShowNumber(uint8_t column, uint8_t page, uint32_t number)
{
    char buffer[11];
    uint8_t index;

    index = 10U;
    buffer[index] = '\0';
    do
    {
        index--;
        buffer[index] = (char)('0' + (number % 10U));
        number /= 10U;
    } while ((number != 0U) && (index > 0U));

    OLED_ShowString(column, page, &buffer[index]);
}
