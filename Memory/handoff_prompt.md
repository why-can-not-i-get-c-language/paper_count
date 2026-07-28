# 项目交接 Prompt

将以下内容复制到新对话中使用。

```text
继续 STM32 纸张计数项目，目录：E:\learn ee\PROJECT\paper_count

开始前必须阅读并遵守：AGENTS.md、Memory/README.md、Memory/architecture.md、Memory/decisions.md、Memory/progress.md、Memory/roadmap.md、Memory/hardware_debug_standard.md、Memory/ui_oled_integration_plan.md、Memory/wiring_map.md。先执行 git status 和 git diff，确认当前状态。

项目目标：两块 50 mm x 50 mm 铜板与纸张形成可变电容；NE555 将电容变化转换为方波频率；STM32F103C8T6 测频，用实测标定表换算约 0~80 张 A4 纸，并在 OLED/串口显示；AT24C08 持久化标定表。

固定约束：使用 STM32F10x Standard Peripheral Library v3.6.0、Keil uVision 和 Arm Compiler 6.24，禁止 HAL；代码注释中文；main.c 仅做初始化和高层调度；PC13 板载 LED 低有效；PA13/PA14 仅供 ST-LINK SWD；所有 STM32 GPIO 禁止接收 5 V；硬件调试说明遵守 hardware_debug_standard.md。

当前仓库与 Git：使用 main 分支，origin 为 GitHub。每个用户确认通过的小任务都必须更新相关 Memory、审查 git diff、只暂存相关文件、创建本地提交并推送。绝不暂存、覆盖或提交 .vscode/c_cpp_properties.json；它是用户本地 VS Code 配置，通常是唯一允许保留的未提交改动。

已验证基础：SysTick、PC13、USART1 PA9 TX 115200（printf 已禁用半主机并重定向）、K1~K4 PB12~PB15、SSD1306 OLED（3.3 V、PB6/PB7、0x3C）、AT24C08（读写、跨页、跨地址块、CRC、测试标定表掉电恢复）、标定插值与稳定判定算法、标定编辑基础 UI、TIM2/PA0 的 1 kHz MCU 回接自测。

当前接线：以 Memory/wiring_map.md 为唯一依据。ST-LINK 是核心板唯一 3.3 V 供电源；USB 串口模块仅接 PA9 -> RXD 与 GND，VCC/TXD 不接；OLED 和 AT24C08 共用 PB6/SCL、PB7/SDA。PA0、PB0、NE555 和蜂鸣器当前未接；PB0 至 PA0 的自测跳线必须保持拆除。

当前启动和 UI：正式程序开机输出 Calibration load status: 3 表示 EEPROM I2C 读取正常但没有真实标定表，是预期状态。监测页 K2 开始编辑；标定页 K2 选 0~80 张、K3 捕获当前频率、K1 放弃编辑、K4 进入保存确认；确认页 K4 保存，失败留在确认页。无真实频率时 K3 已验证安全拒绝；空表第二次 K4 的保存失败路径已验证。真实频率捕获、有效表保存与计数仍待 NE555 联调。

示波器状态：用户已有 DSTouch 示波器并提供中文快速指南和用户手册。探头补偿已完成，PC13 已测得约 0.999 Hz 方波，PA9 串口动态波形已捕获。I2C 首轮波形验证通过：示波器 10X、DC、2 us/格测得 PB6/SCL 约 78.003 kHz，Vmax 3.467 V、Vmin -66.667 mV、Vpp 3.533 V；PB7/SDA Vmax 3.433 V、Vmin -133.333 mV、Vpp 3.567 V；未见超过 3.6 V 或低于 -0.3 V 的稳定尖峰。测量时探头地线必须靠近信号地，不能把探头移动/断开产生的瞬态当作真实过冲。

当前阻塞：NE555 的定时网络、供电、原始输出频率范围和实际连接尚未确认。若 NE555 使用 5 V 输出，禁止直连 PA0；先断开 PA0，用示波器测原始输出，再通过已验证的 5 V 转 3.3 V 电平转换测其输出，确认稳定不超过 3.3 V、没有超过 3.6 V 的尖峰后才接 PA0。一个保守的候选分压器是 NE555 OUT -> 15 kOhm -> 节点 -> PA0，节点 -> 22 kOhm -> GND，且 NE555 GND 必须与 STM32 GND 共地；这只是待实测验证的方案，尚未接线或验收。

蜂鸣器阻塞：器件为 12085P 16 Ohm 插针式无源蜂鸣器，额定 1.5 V、谐振约 2048 Hz、实测直流约 16 Ohm。缺安全驱动所需的三极管、4.7 kOhm 基极电阻、150 Ohm 限流电阻和二极管；蜂鸣器不得直接连接 3.3 V GPIO。

下一步优先级：不要为了等待硬件扩展缺少真实输入的业务代码。等待并核对 NE555 和电平转换器件；器件到位后按 hardware_debug_standard.md 先做断电通断、直流与示波器检查，再接 PA0。之后采集真实纸张的多次稳定频率和环境记录，建立最多 16 点标定表，验证 K3 捕获、K4 保存、掉电恢复和整机计数。NE555 波形、电平转换过冲和传感器噪声必须实际示波器测量后才可宣称通过。
```
