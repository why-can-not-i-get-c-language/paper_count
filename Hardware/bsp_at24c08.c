#include "bsp_at24c08.h"
#include "bsp_soft_i2c.h"
#include "delay.h"

#define AT24C08_I2C_BASE_ADDRESS 0x50U
#define AT24C08_PAGE_SIZE        16U
#define AT24C08_WRITE_TIMEOUT_MS 10U

static uint8_t AT24C08_GetDeviceAddress(uint16_t address, uint8_t read)
{
    uint8_t block;

    block = (uint8_t)((address >> 8U) & 0x03U);
    return (uint8_t)((AT24C08_I2C_BASE_ADDRESS << 1U) | (block << 1U) | (read != 0U ? 1U : 0U));
}

static uint8_t AT24C08_WaitWriteComplete(uint16_t address)
{
    uint8_t timeout;

    for (timeout = 0U; timeout < AT24C08_WRITE_TIMEOUT_MS; timeout++)
    {
        SoftI2C_Start();
        if (SoftI2C_SendByte(AT24C08_GetDeviceAddress(address, 0U)) != 0U)
        {
            SoftI2C_Stop();
            return 1U;
        }
        SoftI2C_Stop();
        Delay_ms(1U);
    }

    return 0U;
}

void AT24C08_Init(void)
{
    SoftI2C_Init();
}

uint8_t AT24C08_Read(uint16_t address, uint8_t *data, uint16_t length)
{
    uint16_t index;

    if ((data == 0) || (length == 0U) || (address >= AT24C08_CAPACITY_BYTES) ||
        (length > (AT24C08_CAPACITY_BYTES - address)))
    {
        return 0U;
    }

    for (index = 0U; index < length; index++)
    {
        SoftI2C_Start();
        if (SoftI2C_SendByte(AT24C08_GetDeviceAddress(address, 0U)) == 0U)
        {
            SoftI2C_Stop();
            return 0U;
        }
        if (SoftI2C_SendByte((uint8_t)address) == 0U)
        {
            SoftI2C_Stop();
            return 0U;
        }

        SoftI2C_Start();
        if (SoftI2C_SendByte(AT24C08_GetDeviceAddress(address, 1U)) == 0U)
        {
            SoftI2C_Stop();
            return 0U;
        }

        /* 每个字节均使用独立随机读事务，读完后发送 NACK 结束本次事务。 */
        data[index] = SoftI2C_ReadByte(0U);
        SoftI2C_Stop();
        address++;
    }

    return 1U;
}

uint8_t AT24C08_Write(uint16_t address, const uint8_t *data, uint16_t length)
{
    uint16_t remaining;
    uint8_t page_remaining;
    uint8_t write_count;
    uint8_t index;

    if ((data == 0) || (length == 0U) || (address >= AT24C08_CAPACITY_BYTES) ||
        (length > (AT24C08_CAPACITY_BYTES - address)))
    {
        return 0U;
    }

    remaining = length;
    while (remaining > 0U)
    {
        page_remaining = (uint8_t)(AT24C08_PAGE_SIZE - (address % AT24C08_PAGE_SIZE));
        write_count = (remaining < page_remaining) ? (uint8_t)remaining : page_remaining;

        SoftI2C_Start();
        if (SoftI2C_SendByte(AT24C08_GetDeviceAddress(address, 0U)) == 0U)
        {
            SoftI2C_Stop();
            return 0U;
        }
        if (SoftI2C_SendByte((uint8_t)address) == 0U)
        {
            SoftI2C_Stop();
            return 0U;
        }
        for (index = 0U; index < write_count; index++)
        {
            if (SoftI2C_SendByte(*data) == 0U)
            {
                SoftI2C_Stop();
                return 0U;
            }
            data++;
        }
        SoftI2C_Stop();

        if (AT24C08_WaitWriteComplete(address) == 0U)
        {
            return 0U;
        }

        address = (uint16_t)(address + write_count);
        remaining = (uint16_t)(remaining - write_count);
    }

    return 1U;
}
