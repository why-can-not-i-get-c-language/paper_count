# 项目路线图

最后更新：2026-08-12

## 阶段状态

| 阶段 | 状态 | 完成条件 |
| --- | --- | --- |
| 软件基础、按键、OLED、EEPROM | 已验证 | Keil 编译与既有硬件测试通过。 |
| NE555、S8050 与 PA0 测频链路 | 已验证基线 | 示波器确认波形，PA0 串口持续读到稳定频率。 |
| 机械结构 | 待设计 | 铜板平行、间距和压力可重复。 |
| 电子 PCB | 待设计 | 原理图、封装、DRC、焊接后通电复测通过。 |
| 真实纸张标定 | 暂缓 | 等待机械夹具；当前不使用手工夹持数据建立标定表。 |
| 整机验收 | 待开始 | 标定、掉电恢复、计数和异常处理通过。 |

## 实施顺序

1. 保留当前面包板作为电子基线，确认 PA0 测频仍正常。
2. 按 `hardware_package_inventory.md` 逐项复核核心板排母、模块接口、插件器件和机械孔位。
3. 设计整板原理图：集成核心板插座、NE555、两只 S8050、阻容、蜂鸣器、OLED、AT24C08、按键、电源和测试点；两块铜板只通过可拆接口连接。
4. 至少保留定时电阻、定时电容的可替换焊盘或插座；最终阻容值未因纸张测试而冻结。
5. 设计 3D 打印机械夹具：两块 50mm x 50mm 铜板平行、间距可调、绝缘、压力可重复、导线有固定和应力释放。
6. PCB 焊接后先不接 PA0，复测 NE555 `3脚 OUT` 和 S8050 `C`；通过后再接 PA0。
7. 在夹具中测试空载、1、5、10 张及更多纸张，每种状态多次记录串口和示波器频率。
8. 根据数据冻结阻容和机械尺寸，建立最多 16 个真实标定点，再验证 K3 捕获、K4 保存、掉电恢复和整机计数。

## 整板原理图接线基线

以下接线用于绘制“一块主 PCB”的完整原理图。原理图中的网络名必须保持一致；实际封装、孔径、排母方向和模块机械尺寸仍以 `Memory/hardware_package_inventory.md` 的最终复核为准。

### 1. 电源网络

```text
核心板 Type-C 输入 -> 核心板内部电源
核心板 +5V 引脚 -> 主板 +5V
核心板 3V3 引脚 -> 主板 +3V3
核心板 GND 引脚 -> 主板 GND
```

```text
+5V -> NE555 U1 pin 8 VCC
+5V -> NE555 U1 pin 4 RESET
+5V -> 蜂鸣器限流电阻 R6
+3V3 -> Q1 集电极上拉 R5
+3V3 -> OLED VCC
+3V3 -> AT24C08 U2 VCC
GND -> 所有模块公共地
```

`+5V` 和 `+3V3` 必须是两个不同网络，不得短接。主 PCB 不新增独立稳压器；Type-C 供电和核心板 5V/3.3V 输出能力需在实物复核时再次确认。

### 2. STM32 核心板插接接口

主 PCB 放置两排可拆 `1x20` 排母，核心板插入排母，不直接焊接 STM32 LQFP-48 芯片。只使用下列核心板信号，其他排针脚保持未连接或按实物丝印确认：

```text
核心板 +5V  -> 主板 +5V
核心板 3V3  -> 主板 +3V3
核心板 GND  -> 主板 GND
核心板 PA0  -> NET_PA0
核心板 PB1  -> NET_BUZZER_PWM
核心板 PB6  -> NET_I2C_SCL
核心板 PB7  -> NET_I2C_SDA
核心板 PB12 -> NET_KEY1
核心板 PB13 -> NET_KEY2
核心板 PB14 -> NET_KEY3
核心板 PB15 -> NET_KEY4
```

`PA13`、`PA14` 不接主板外设，保留给外部 ST-LINK。下载器、USB 串口和 Type-C 插座不放在主 PCB 上。

### 3. NE555 测频网络

U1 使用插件 DIP-8，缺口/圆点方向必须在丝印标出：

```text
U1 pin 1 GND    -> GND
U1 pin 2 TRIG   -> NET_TIMING
U1 pin 3 OUT    -> R3 10kOhm -> NET_Q1_BASE
U1 pin 4 RESET  -> +5V
U1 pin 5 CONT   -> C2 10nF -> GND
U1 pin 6 THRES  -> NET_TIMING
U1 pin 7 DISCH  -> R1 10kOhm -> +5V，并经 R2 100kOhm 接 NET_TIMING
U1 pin 8 VCC    -> +5V
```

```text
NET_TIMING -> C1 470pF -> GND
NET_TIMING -> TP_SENSOR_A（带铜焊盘）-> 外部铜板 A
TP_SENSOR_B（带铜焊盘）-> GND -> 外部铜板 B
```

`C_SENSOR` 只在原理图中表示外部两块铜板形成的可变电容，不分配 PCB 电容封装；PCB 实际放置 `TP_SENSOR_A`、`TP_SENSOR_B` 两个带铜环的插件焊线通孔。`R1`、`R2`、`C1` 保留可替换插件焊盘。

### 4. Q1 测频电平转换

Q1 为测频专用 S8050，采用资料确认的 TO-92 底视图 `1=E、2=B、3=C`，丝印标出 `E/B/C`：

```text
Q1 E -> GND
Q1 B -> NET_Q1_BASE
Q1 B -> R4 100kOhm -> GND
Q1 C -> NET_Q1_COLLECTOR
NET_Q1_COLLECTOR -> R5 4.7kOhm -> +3V3
NET_Q1_COLLECTOR -> NET_PA0 -> 核心板 PA0
```

NE555 U1 pin 3 不得直接连接 `NET_PA0`。

### 5. 蜂鸣器驱动

Q2 为蜂鸣器专用第二只 S8050，不能与 Q1 共用：

```text
核心板 PB1 -> NET_BUZZER_PWM -> R7 4.7kOhm -> Q2 B
Q2 B -> R8 100kOhm -> GND
Q2 E -> GND
Q2 C -> NET_BUZZER_LOW
+5V -> R6 150Ohm -> BZ1 正极（外壳 +）
BZ1 负极 -> NET_BUZZER_LOW
```

D1 为插件 1N4148，反向并联在蜂鸣器两端：

```text
D1 阴极（有色环） -> BZ1 正极 / R6 后端
D1 阳极 -> BZ1 负极 / NET_BUZZER_LOW
```

### 6. OLED 接线

OLED-01 使用 3.3V I2C 模块，原理图网络如下：

```text
OLED GND -> GND
OLED VCC -> +3V3
OLED SCL -> NET_I2C_SCL -> 核心板 PB6
OLED SDA -> NET_I2C_SDA -> 核心板 PB7
```

模块四针顺序以实物丝印 `GND、VCC、SCL、SDA` 为准，PCB 开窗和安装孔按实物尺寸复核。

### 7. AT24C08 接线

U2 使用 DIP-8 或对应 IC 座，芯片缺口朝上时按资料确认编号：

```text
U2 pin 1 A0  -> 悬空
U2 pin 2 A1  -> 悬空
U2 pin 3 A2  -> GND
U2 pin 4 GND -> GND
U2 pin 5 SDA -> NET_I2C_SDA -> 核心板 PB7
U2 pin 6 SCL -> NET_I2C_SCL -> 核心板 PB6
U2 pin 7 WP  -> GND
U2 pin 8 VCC -> +3V3
```

### 8. 四个按键

每个按键的一端接对应 STM32 网络，另一端统一接 GND；软件使用内部上拉：

```text
KEY1 一端 -> NET_KEY1 -> 核心板 PB12；另一端 -> GND
KEY2 一端 -> NET_KEY2 -> 核心板 PB13；另一端 -> GND
KEY3 一端 -> NET_KEY3 -> 核心板 PB14；另一端 -> GND
KEY4 一端 -> NET_KEY4 -> 核心板 PB15；另一端 -> GND
```

四脚插件按键的内部两两连通方向必须按实物万用表确认，不能只按外观猜测；PCB 旋转方向和丝印需明确。

### 9. 测试点和外部焊盘

主板建议放置带网络的插件测试焊盘：

```text
TP_5V          -> +5V
TP_3V3         -> +3V3
TP_GND         -> GND
TP_TIMING      -> NET_TIMING
TP_NE555_OUT   -> U1 pin 3 / NET_NE555_OUT
TP_Q1_COL      -> NET_Q1_COLLECTOR / NET_PA0
TP_SENSOR_A    -> NET_TIMING / 铜板 A 焊线孔
TP_SENSOR_B    -> GND / 铜板 B 焊线孔
TP_PA0         -> NET_PA0
```

`TP_SENSOR_A`、`TP_SENSOR_B` 是实际焊线通孔，必须有铜环和网络连接；四角 M4 安装孔使用 NPTH 无铜机械孔，不属于上述测试点。

### 10. 原理图绘制检查顺序

1. 先放置核心板两排 `1x20` 排母和 U1/Q1/Q2/U2。
2. 建立 `+5V`、`+3V3`、`GND` 三个电源网络。
3. 按本节顺序完成 NE555、Q1、蜂鸣器、OLED、AT24C08 和按键接线。
4. 放置 `TP_SENSOR_A/B`、`TP_PA0`、`TP_TIMING` 等测试点。
5. 每完成一个模块，检查网络标签是否完全一致，尤其是 `NET_TIMING`、`NET_PA0`、`NET_I2C_SCL`、`NET_I2C_SDA`。
6. 在未确认核心板排针方向、按键脚位、BZ1 极性孔位和电容尺寸前，只完成原理图，不进入最终封装和下单。

`R1=10kOhm`、`R2=100kOhm`、`C1=470pF` 仍是当前可工作的初始值，不是最终标定值；`1N4148` 仅用于蜂鸣器驱动，不接入 NE555 定时网络。

## PCB 边界

- 应放入 PCB：NE555、S8050、全部固定/可替换阻容、5V/3.3V/GND 接线端子、SENSOR 接口、PA0 接口、示波器测试点。
- 不应直接焊入主 PCB：两块 50mm x 50mm 铜板、纸张夹具和 3D 结构件。铜板应通过短线和可拆接插件连接，便于调整间距和更换机械结构。
- PCB 需要按实际封装建库：插件 NE555 DIP-8、插件电阻电容、S8050 的实际 `E/B/C` 排列；封装方向必须在丝印上标出。
- 当前 PCB 目标为一块整板；STM32 仍以现成核心板可拆插接，主 PCB 不直接焊接 LQFP-48 芯片。所有封装以 `hardware_package_inventory.md` 为准，未复核项目不得下单。
- 使用嘉立创 EDA 专业版逐步设计，采用普通双层板、常规板材和常规工艺，板框控制在 100mm x 100mm 以内。免费打板属于下单活动条件，提交前仍需按当时订单页面逐项核对。
- 本轮暂不设计 3D 机械夹具，但铜板接口必须保持可拆，不能妨碍后续夹具设计。
- 核心板通过主 PCB 两排可拆排母插接；两块铜板和机械夹具仍不得直接焊死在主 PCB。
- 主 PCB 设置机械安装孔；当前用户已有 M4、160 mm 螺纹螺杆，因此孔径、铜环和板边距离需在 PCB 封装/板框阶段按实际 M4 五金件核对。安装孔默认无铜、无网络连接。
- 整板方案下不再规划第二块电子 PCB；机械夹具仍作为独立结构设计。
