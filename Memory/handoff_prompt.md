# 项目交接 Prompt

将以下内容完整复制到新的 Codex 对话中使用。

```text
你正在继续一个 STM32 纸张计数项目。项目目录为：
E:\learn ee\PROJECT\paper_count

请先阅读项目根目录的 AGENTS.md，以及 Memory/architecture.md、Memory/decisions.md、Memory/progress.md。

项目目标：
使用两块铜箔极板和纸张形成可变电容；NE555 将电容变化转换为方波频率；STM32F103C8T6 测频并基于实测标定表换算纸张数量，最终通过 OLED 和串口显示。

固定技术约束：
- MCU：STM32F103C8T6。
- 工具链：Keil uVision，Arm Compiler 6.24（ARMCLANG）。
- 库：STM32F10x Standard Peripheral Library v3.6.0，禁止改用 HAL。
- 代码注释必须使用中文。
- PC13 是板载 LED，低电平点亮。
- PA13、PA14 保留给 ST-LINK。
- NE555 的 5 V 输出进入 STM32 前必须降至 3.3 V。

已通过真实硬件验证：
1. PC13 LED 每约 0.5 秒闪烁。
2. SysTick 延时正常。
3. USART1 调试输出正常：PA9 为 TX，115200 bps；启动信息和每秒心跳可在串口助手看到。
4. ARMCLANG 下 printf 已处理半主机问题：bsp_usart.c 使用
   __asm(".global __use_no_semihosting");
   并通过 fputc 重定向到 USART1。不要删除该处理。

当前按键状态：
- K1-K4 使用 PB12、PB13、PB14、PB15 的内部上拉输入，按下接 GND。
- `bsp_key.c` 已加入 Keil，且 K1-K4 已完成真实硬件验证。
- 已验证：Keil 编译通过；PB12-PB15 未按下约 3.3 V、按下约 0 V；串口按键事件与板载 LED 均正常。
- 后续硬件接线、万用表检查和软件验证必须遵循 `Memory/hardware_debug_standard.md`。

当前软件状态：
- 已验证：System/delay.c/.h、Hardware/bsp_led.c/.h、Hardware/bsp_usart.c/.h。
- 已验证：Hardware/bsp_key.c/.h。
- 未实现：软件 I2C、OLED、AT24C08、NE555 测频、标定算法、纸张计数、UI。
- 当前优先执行路线图阶段 0 的硬件信息确认，以及阶段 1 的软件工作，例如 Application/app_calibration.c/.h 的频率标定表、最近值查找和线性插值算法。

Git 状态（交接时）：
- 分支：main，比 origin/main 领先 1 个提交。
- 最近本地提交：8198e2b feat: add verified USART1 debug output。
- 按键阶段已完成；继续工作前先检查当前 Git 状态与最新项目记录。
- 另有未提交的 .vscode/c_cpp_properties.json 改动，不属于按键任务；不要随意暂存、覆盖或提交它。
- 默认不推送远程仓库，除非用户明确要求。

工作规则：
1. 每次先检查当前文件和 git diff，保留用户已有改动。
2. 只修改当前任务相关文件。
3. 每一步先通过 Keil 编译和用户硬件测试，再更新 Memory 状态。
4. 只有完成预期硬件验证的模块才能标记“已验证”。
5. 在任务被用户接受后，检查 git diff，仅暂存当前任务相关文件，更新 Memory，创建清晰的本地提交；默认不推送。
6. 向用户提供 Keil 操作、接线和测试步骤时使用保姆级中文说明。

请先简要汇报你读取到的当前状态，再等待或执行用户指定的下一项工作。
```
