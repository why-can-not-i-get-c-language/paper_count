# 项目交接 Prompt

将以下内容复制到新对话中使用。

```text
继续 STM32 纸张计数项目，目录：E:\learn ee\PROJECT\paper_count

先阅读并遵守：AGENTS.md、Memory/architecture.md、Memory/decisions.md、Memory/progress.md、Memory/roadmap.md、Memory/hardware_debug_standard.md、Memory/ui_oled_integration_plan.md、Memory/wiring_map.md。

目标：两块 50 mm x 50 mm 铜板与纸张形成可变电容；NE555 将变化转成频率；STM32F103C8T6 测频，用实测标定表换算约 0~80 张 A4 纸，并在 OLED/串口显示；AT24C08 保存标定表。

固定约束：STM32F10x SPL v3.6.0，Keil uVision + Arm Compiler 6.24，禁止 HAL；代码注释中文；main.c 只做初始化和高层调度；PC13 LED 低有效；PA13/PA14 保留 SWD；GPIO 禁止接收 5 V；硬件说明遵守 hardware_debug_standard.md。

已实测：SysTick、PC13、USART1 PA9 TX 115200、K1~K4 PB12~PB15、OLED SSD1306（3.3 V、PB6/PB7、0x3C）、TIM2 PA0 1 kHz MCU 回接测频、AT24C08 读写/跨页/掉电恢复、UI 空表保存失败提示。I2C 首轮示波器验证通过：PB6/SCL 约 78.003 kHz，Vmax 3.467 V、Vmin -66.667 mV；PB7/SDA Vmax 3.433 V、Vmin -133.333 mV，未见超过 3.6 V 或低于 -0.3 V 的稳定尖峰。面包板已重新整理并恢复正常；ST-LINK 是核心板唯一 3.3 V 供电，USB 串口仅接 `PA9 -> RXD` 和 GND。PB0 至 PA0 自测跳线必须保持拆除。

当前软件：标定编辑基础 UI 已验证。规则为：监测页 K2 开始编辑；标定页 K2 选 0~80 张、K3 捕获当前频率、K1 放弃编辑、K4 进入确认；确认页 K4 保存，失败留在确认页。当前无真实频率时 K3 已验证安全拒绝；真实频率捕获待 NE555 联调。

硬件阻塞：蜂鸣器是 12085P 16 Ω 无源件，额定 1.5 V、实测直流约 16 Ω；驱动三极管、限流电阻和续流二极管未到位，任务暂缓，禁止直连 3.3 V GPIO。NE555 定时网络、电平转换和真实纸张标定未完成。硬件未到齐时仅维护已验证基线、核对元件规格并准备标定记录，不扩展缺少真实输入的业务代码。

波形边界：用户已使用示波器完成 I2C 首轮验证。NE555 波形、电平转换过冲和铜板传感器噪声仍须在相应硬件接入后实测；未经测量不得宣称完成。

Git：分支 main，远程 origin 指向 GitHub；每个用户确认通过的小任务都要更新 Memory、审查差异、只暂存相关文件、创建本地提交并推送。绝不暂存、覆盖或提交 .vscode/c_cpp_properties.json。开始前先检查 git status 和 git diff。
```
