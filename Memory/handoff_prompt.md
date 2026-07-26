# 项目交接 Prompt

将以下内容完整复制到新的 Codex 对话中使用。

```text
你正在继续 STM32 纸张计数项目，目录为：
E:\learn ee\PROJECT\paper_count

第一步必须阅读并遵守：
1. 根目录 AGENTS.md。
2. Memory/architecture.md。
3. Memory/decisions.md。
4. Memory/progress.md。
5. Memory/roadmap.md。
6. Memory/hardware_debug_standard.md。
7. Memory/ui_oled_integration_plan.md。

项目目标：两块 50mm x 50mm 铜板与纸张形成可变电容；NE555 将电容变化转为频率；STM32F103C8T6 测频，通过实测标定表换算 0~80 张普通 A4 纸的粗略数量，并在 OLED、串口显示；AT24C08 保存标定数据。

固定约束：
- MCU：STM32F103C8T6；库：STM32F10x Standard Peripheral Library v3.6.0；禁止 HAL。
- 工具链：Keil uVision + Arm Compiler 6.24。
- C 代码注释使用中文；main.c 仅负责初始化和高层调度。
- PC13 板载 LED 为低电平点亮；PA13/PA14 保留 ST-LINK。
- STM32 GPIO 不得接收 5V。最终 NE555 使用 5V 时，OUT 必须先完成 5V 到 3.3V 电平转换再进入 PA0。
- 每次硬件接线、万用表检查和测试说明必须遵循 Memory/hardware_debug_standard.md：明确器件、板上丝印、线材、断电检查、上电电压和软件预期。

已完成真实硬件验证：
1. SysTick 延时、PC13 LED、USART1（PA9 TX，115200bps）正常；ARMCLANG printf 已禁用半主机并重定向 USART1，不要删除 bsp_usart.c 中的处理。
2. K1-K4：PB12/PB13/PB14/PB15 内部上拉，按下接 GND；未按约3.3V、按下约0V，事件与 LED/串口均正常。
3. OLED：0.96英寸 SSD1306、128x64，3.3V 供电；PB6=SCL、PB7=SDA，7位地址0x3C；显示、ACK、当前字库均正常。
4. UI 联合验证：监测页、标定页、保存确认页及顺序/乱序按键规则均正确；后台频率更新不会覆盖非监测页文字。
5. 测频自测：TIM3_CH3/PB0 输出约1kHz方波回接 TIM2_CH1/PA0 后，串口稳定显示 Frequency: 1000 Hz。PB0到PA0的自测跳线已经必须拆除；Freq_SelfTestOutputInit 仅供以后自测，不在 main 默认调用。

当前代码状态：
- 已验证：delay、LED、USART、按键、软件I2C、OLED、UI、TIM2/PA0 测频自测。
- 已算法和 Keil 编译验证：app_calibration（最多16标定点、升序校验、整数分段线性插值），app_paper_counter（稳定样本、容差、状态映射）。
- 未实现：AT24C08、蜂鸣器、最终 NE555 传感器电路和真实纸张标定/整机联调。
- OLED 当前字库只覆盖当前页面所需的大写字母、数字、空格和冒号；新增显示文本前应补充字库并测试。
- UI 按键规则见 ui_oled_integration_plan.md；无效页面操作不改变页面，并通过串口输出 UI action ignored。

当前暂停和锁定：
- 用户要求暂停任何需要示波器或逻辑分析仪的任务。
- 锁定范围：NE555 波形幅度/占空比/边沿/毛刺，5V到3.3V转换过冲，I2C波形质量，铜板传感器噪声波形。
- 未经用户明确解锁，不得主动开展或标记这些事项为已验证；可继续软件、Keil、MCU回接自测、万用表静态检查。

当前下一步候选：
1. 收集并确认 AT24C08 模块/芯片的封装、A0/A1/A2、WP、供电和引脚，再实现 EEPROM 驱动与标定表保存。
2. 在元件到位后设计 NE555 固定电容振荡器和 5V到3.3V电平转换，先做万用表静态检查，再接 PA0；波形质量检查保持锁定。

Git 规则：
- 当前分支 main；最近提交 d969361 feat: integrate ui with oled controls。
- .vscode/c_cpp_properties.json 是用户的未提交改动，绝不暂存、覆盖或提交；继续前先检查 git status 和 git diff。
- 每个小任务经用户 Keil/硬件验证后，更新 Memory，审查 diff，只暂存直接相关文件，创建本地提交；默认不推送。

开始工作前，先用中文简要列出你已读取并将据此遵守的信息，供用户审查；至少包括：项目目标、工具链/代码约束、已验证模块及引脚、当前未完成项、波形任务锁定规则、Git 未提交文件和你理解的下一步。完成该清单后再等待或执行用户的下一项指令。
```
