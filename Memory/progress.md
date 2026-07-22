# 项目进度

最后更新：2026-07-22

## 当前阶段

阶段 3：按键输入验证

## 已完成并验证

- 已按标准外设库建立项目目录和库文件结构。
- 已建立 Keil 所需的启动文件、CMSIS 文件和 SPL 源文件。
- `System/delay.c/.h` 已通过板载运行验证，SysTick 毫秒延时正常。
- `Hardware/bsp_led.c/.h` 已通过板载运行验证，PC13 每隔约 0.5 秒闪烁。
- 已确认核心板 PWR 红色 LED 常亮，供电正常。
- 已初始化本地 Git 仓库，并建立忽略规则、提交规范和项目 Memory 区域。
- USART1 调试输出已通过硬件验证：启动信息、每秒心跳输出和 PC13 LED 闪烁均正常。

## 当前待做

1. 实现按键 GPIO 初始化和读取函数。
2. 使用串口输出验证四个按键的按下事件。

## 本阶段排查记录

- 已验证：仅恢复 `USART1_Init(115200U)`，且不执行发送函数或 `printf` 时，PC13 LED 仍正常闪烁。
- 结论：USART1 初始化路径正常；排查重点转为发送函数和 `printf` 重定向路径。
- 已验证：使用带 TXE 超时的 `USART1_SendString("USART test\r\n")` 后，PC13 LED 正常闪烁，串口正确接收文本。
- 结论：USART1 直接发送路径正常；当前只验证 `printf` 重定向。
- 已验证：使用 `printf("printf test\r\n")` 后，PC13 LED 停止闪烁且串口无输出。
- 结论：问题位于 Keil C 库的 `printf` 输出路径，推定为半主机输出阻塞；直接 USART 发送功能未受影响。
- 已验证：Arm Compiler 6.24 不支持 Arm Compiler 5 的 `#pragma import`，且其标准库已定义 `struct __FILE`。
- 结论：半主机禁用代码必须按 Arm Compiler 6 的接口实现，不能直接使用旧版 Keil 示例。
- 已验证：采用 Arm Compiler 6 的 `__use_no_semihosting` 符号后，`printf("printf test\r\n")` 正常输出，PC13 LED 正常闪烁。
- 结论：半主机阻塞已消除，`printf` 重定向路径正常。
- 已验证：启动信息、每秒一次的串口心跳输出和 PC13 LED 闪烁均正常。
- 结论：USART1 调试模块完成，可用于后续外设的串口调试。

## 后续计划

1. 实现按键模块并验证按键读取。
2. 实现软件 I2C 和 OLED 显示。
3. 实现 NE555 方波测频。
4. 实现频率-张数标定与滤波。
5. 实现 EEPROM 保存标定表。

## 阻塞项

- 无已知阻塞项。
