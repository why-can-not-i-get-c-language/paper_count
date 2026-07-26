# Memory

此目录保存项目的长期工作记忆，不存放会参与 Keil 编译的 C 源码。

## 文件说明

- `architecture.md`：系统分层、模块责任和模块依赖。
- `decisions.md`：已经确认的技术选择及其原因。
- `progress.md`：当前工作状态、下一步和待确认事项。
- `roadmap.md`：按依赖关系排列的项目实施顺序、阶段目标和完成条件。
- `hardware_debug_standard.md`：硬件接线、万用表检查和软件验证的统一说明标准。
- `ui_oled_integration_plan.md`：按键、UI 状态、OLED 与串口联合验证路径。
- `wiring_map.md`：用户确认已经实际接线并有效的接口总表。
- `handoff_prompt.md`：用于新对话或新代理的项目交接 Prompt。

## 更新规则

1. 新增或调整模块责任时，更新 `architecture.md`。
2. 做出会影响后续代码或硬件的决定时，更新 `decisions.md`。
3. 每完成一个可验证的阶段后，更新 `progress.md`。
4. 只有通过 Keil 编译并完成对应硬件测试的功能，才能标记为“已验证”。
