# 项目交接 Prompt

继续 STM32 纸张计数项目，目录：`E:\learn ee\PROJECT\paper_count`。

开始前阅读：`AGENTS.md`、`Memory/README.md`、`Memory/architecture.md`、`Memory/decisions.md`、`Memory/progress.md`、`Memory/roadmap.md`、`Memory/hardware_debug_standard.md`、`Memory/ui_oled_integration_plan.md`、`Memory/wiring_map.md`，并执行 `git status`、`git diff`。

项目目标：两块 50mm x 50mm 铜板与纸张形成可变电容；NE555 将变化转换为方波；S8050 将 5V 输出转换为 3.3V 逻辑；STM32F103C8T6 通过 TIM2/PA0 测频，用真实标定表换算约 0~80 张 A4 纸，并在 OLED/串口显示；AT24C08 保存标定表。

当前硬件基线：NE555 `1 GND`、`8 VCC=约5V`、`4 RESET=5V`、`2/6` 为定时节点、`3 OUT` 经 10kOhm 接 S8050 `B`；S8050 `E->GND`、`B->100kOhm->GND`、`C->4.7kOhm->STM32 3.3V`、`C->PA0`。定时网络为 `5V->10kOhm->7脚->100kOhm->2/6节点`，节点接 470pF 和铜板 A，铜板 B 接 GND；5脚接 10nF 到 GND，电源端接 100nF 与 10uF 去耦。S8050 的 E/B/C 排列必须按实际器件资料确认。

当前验证：NE555、S8050、PA0 测频链路已通过；串口持续输出约 13kHz。空载、1张、5张纸的手工夹持数据区间重叠，尚未标定。下一步先设计独立 NE555 前端 PCB；核心板、OLED、AT24C08、按键和蜂鸣器所在板后续再设计。两块板沿用 2.54 mm 杜邦线连接，NE555 板四角预留机械安装孔。本轮暂不设计机械夹具。

固定约束：使用 STM32F10x Standard Peripheral Library，不用 HAL；PA13/PA14 只供 ST-LINK；STM32 GPIO 禁止 5V；蜂鸣器不得直接接 GPIO；实际接线只记录在 `wiring_map.md`。

首版 PCB 只放 NE555 前端、S8050、阻容、端子和测试点；通过 2.54 mm 杜邦线连接后续核心板。两块铜板和机械夹具不直接焊死在主 PCB。NE555 板四角设置仅用于机械固定的安装孔。嘉立创下单时需分别选择“仅 PCB 制板”或“PCB+贴片/插件装配”，详见当前对话结论。
