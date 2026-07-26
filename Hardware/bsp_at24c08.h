#ifndef __BSP_AT24C08_H
#define __BSP_AT24C08_H

#include <stdint.h>

#define AT24C08_CAPACITY_BYTES 1024U

/* 初始化 AT24C08 使用的 I2C 总线。 */
void AT24C08_Init(void);

/* 从 EEPROM 连续读取数据，成功返回 1，失败返回 0。 */
uint8_t AT24C08_Read(uint16_t address, uint8_t *data, uint16_t length);

/* 向 EEPROM 连续写入数据，自动处理 16 字节页边界，成功返回 1，失败返回 0。 */
uint8_t AT24C08_Write(uint16_t address, const uint8_t *data, uint16_t length);

#endif
