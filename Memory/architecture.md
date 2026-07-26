# 项目架构与工作责任

## 目标

测量两块铜箔极板夹入纸张后产生的电容变化。NE555 将电容变化转换为方波频率，STM32 测量频率并通过标定表换算纸张数量，最后在 OLED 和串口显示结果。

## 数据流

```text
纸张与极板 -> NE555 方波 -> STM32 定时器测频 -> 滤波与稳定判断
-> 标定表换算张数 -> OLED/串口显示
                       -> AT24C08 保存或读取标定表
```

## 目录责任

| 目录 | 责任 | 不应承担的责任 |
| --- | --- | --- |
| `User/` | 主程序初始化、中断入口、全局板级配置 | 具体外设时序和纸张算法 |
| `System/` | 延时等通用基础服务 | 具体硬件设备驱动 |
| `Hardware/` | GPIO、串口、OLED、EEPROM、按键、蜂鸣器、测频等驱动 | 纸张数量换算规则 |
| `Application/` | 标定、计数、页面状态和业务流程 | 直接操作寄存器 |
| `Memory/` | 设计与进度记录 | Keil 编译源文件 |

## 模块责任

| 模块 | 文件 | 责任 | 状态 |
| --- | --- | --- | --- |
| 延时 | `System/delay.c/.h` | SysTick 毫秒计时与阻塞延时 | 已验证 |
| 板载 LED | `Hardware/bsp_led.c/.h` | PC13 LED 初始化和控制 | 已验证 |
| 串口 | `Hardware/bsp_usart.c/.h` | USART1 调试信息输出 | 已验证 |
| 按键 | `Hardware/bsp_key.c/.h` | PB12-PB15 按键读取与消抖 | 已验证 |
| 软件 I2C | `Hardware/bsp_soft_i2c.c/.h` | PB6/PB7 OLED 与 EEPROM 的 I2C 时序 | 已通过 OLED 硬件验证 |
| OLED | `Hardware/bsp_oled.c/.h` | SSD1306 OLED 初始化与状态显示 | 已验证 |
| EEPROM | `Hardware/bsp_at24c08.c/.h` | 保存和读取标定数据 | 未实现 |
| 测频 | `Hardware/bsp_freq.c/.h` | TIM2/PA0 输入捕获读取方波频率 | 已通过 STM32 1 kHz 回接自测，待 NE555 联调 |
| 标定 | `Application/app_calibration.c/.h` | 管理频率-张数标定数据与分段线性插值 | 算法与 Keil 编译已验证，真实标定待联调 |
| 计数 | `Application/app_paper_counter.c/.h` | 频率稳定判断与标定换算 | 算法与 Keil 编译已验证，真实频率待联调 |
| UI | `Application/app_ui.c/.h` | 页面状态、按键事件与显示数据 | 已通过按键与 OLED 联合硬件验证 |

## 已确认硬件约束

- MCU 为 STM32F103C8T6。
- PC13 是核心板板载 LED，通常为低电平点亮。
- PA13、PA14 保留给 ST-LINK 调试。
- NE555 输出进入 STM32 前必须从 5 V 降至 3.3 V。
- K1-K4 分别连接 PB12、PB13、PB14、PB15，按下时接地。
- OLED 为 0.96 英寸、128x64、白色单色 I2C 模块，驱动芯片为 SSD1306。
- OLED 排针从正面、排针在上方时由左至右为 GND、VCC、SCL、SDA。
- OLED 使用 3.3 V 供电，SCL/SDA 逻辑电平不得超过 3.3 V。
- OLED 默认 7 位 I2C 地址为 0x3C；模块可配置为 0x3D。
- OLED 模块板载 SCL/SDA 至 VCC 的 4.7 kΩ 上拉电阻。

## 待确认硬件信息

- OLED 的驱动芯片、I2C 地址和实际引脚。
- AT24C08 的 A0、A1、A2 地址引脚连接方式。
- 四个按键、LED、蜂鸣器的实际引脚。
- NE555 与 STM32 测频引脚的最终连接方式。
