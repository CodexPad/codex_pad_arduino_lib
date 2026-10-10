# CodexPad Arduino Lib

[![Arduino ESP32 Build](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32_build.yml/badge.svg)](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32_build.yml)
[![Arduino ESP32-C3 Build](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32c3_build.yml/badge.svg)](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32c3_build.yml)
[![Arduino ESP32-C5 Build](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32c5_build.yml/badge.svg)](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32c5_build.yml)
[![Arduino ESP32-C6 Build](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32c6_build.yml/badge.svg)](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32c6_build.yml)
[![Arduino ESP32-H2 Build](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32h2_build.yml/badge.svg)](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32h2_build.yml)
[![Arduino ESP32-S3 Build](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32s3_build.yml/badge.svg)](https://github.com/CodexPad/codex_pad_arduino_lib/actions/workflows/arduino_esp32s3_build.yml)

[English](README.md)

## 概述

本库为**CodexPad**系列手柄提供的Arduino库，支持ESP32系列开发板通过蓝牙连接并读取手柄的所有按键与摇杆输入状态。关于手柄产品的详细信息，请查阅以下产品文档。

| CodexPad型号 | 详情 |
| :--- | :--- |
| CodexPad-C10 | [产品详情](../../../codex_pad_c10/blob/main/README.zh-CN.md#codexpad-c10) |
| CodexPad-S10 | [产品详情](../../../codex_pad_s10/blob/main/README.zh-CN.md#codexpad-s10) |

## 支持的硬件平台

| 支持的硬件平台 |
| :--- |
| ESP32 |
| ESP32-S3 |
| ESP32-C3 |
| ESP32-C5 |
| ESP32-C6 |
| ESP32-H2 |

## 特性

- **灵活的双模式连接**：
  - **地址直连**：通过已知的地址，快速与指定手柄建立稳定连接。
  - **按键掩码连接**：无需提前知道地址。通过扫描并匹配目标手柄上被按住的、由用户代码自定义的按键组合（即“按钮掩码”），自动连接信号最强（RSSI最大）的设备，实现快速、灵活的配对。
- **按键事件检测**：可实时读取所有按键的输入状态，并区分**按下**、**释放**和**按住**三种事件。
- **高精度摇杆数据**：获取左右摇杆X轴和Y轴的模拟量数值，范围从0至255，提供精准的控制输入。

## 使用说明

### 准备工作

在开始编程前，请完成以下准备工作，以确保开发过程顺利进行。

#### 熟悉产品文档

- 详细阅读手柄产品手册，全面了解硬件特性、熟悉手柄按键摇杆布局、功能定义、指示灯状态以及开关机操作等基本信息。

#### 获取并记录手柄地址

> **⚠️ 重要提示**：本库直连的示例是通过地址进行连接。**编程时，必须在代码明确指定您手柄的地址。**

请参考产品手册中提供的方法，获取您手柄的地址。其格式通常为 `"E4:66:E5:A2:24:5D"`（由0-9、A-F的字符组成，冒号为半角）。请妥善记录此信息，后续需要在代码为您自己手柄的实际地址。

#### 开启手柄并进入待连接状态

- 将手柄开机，手柄开机后会自动处于蓝牙可被发现的**待连接状态**，此时手柄指示灯应呈现**慢闪状态（约每秒闪烁一次）**。

### 安装 ESP32 开发板管理器

1. 在 Arduino IDE 中，打开**工具** > **开发板** > **开发板管理器...**。
2. 在搜索框中输入 `esp32`，找到并安装 **ESP32 by Espressif Systems**。
3. 安装完成后，在**工具** > **开发板**列表中选择您使用的具体 ESP32 开发板型号（如 `ESP32 Dev Module`）。
4. 通过 USB 数据线将开发板连接至电脑，并在**工具** > **端口**菜单中选择正确的串行端口。

### 安装CodexPad库

1. **打开 Arduino IDE 库管理器**
   - 菜单栏：**工具** → **管理库...**
   - 快捷键：`Ctrl+Shift+I`（Windows/Linux）或 `Cmd+Shift+I`（Mac）

2. **搜索并安装**
   - 在搜索框中输入：`CodexPad`
   - 找到 CodexPad 库
   - **确保在下拉菜单中选择最新版本**
   - 点击 **安装** 按钮

    ![在库管理中搜索 CodexPad](assets/images/zh-CN/install_codexpad_library.png)

    > **📌 注意：** 截图仅供参考。请务必安装最新可用版本。

3. **安装依赖库**
   - 当出现依赖库安装对话框时，选择 **全部安装**

    ![安装依赖确认对话框](assets/images/zh-CN/install_dependencies_dialog.png)

> **⚠️ 重要版本说明**  
> 本文档中的截图可能显示较旧版本。**请始终安装以下库的最新版本**：
>
> - `CodexPad` 库
> - `NimBLE-Arduino` 依赖库
> - `GamepadInput` 依赖库
> - `cyfney-cpp` 依赖库
>
> 如果您跳过了依赖库安装，请手动安装最新版 `NimBLE-Arduino`、`GamepadInput` 库和`cyfney-cpp`库：
>
> 1. 再次打开库管理器
> 2. 搜索 `NimBLE-Arduino` 或者 `GamepadInput` 或者 `cyfney-cpp`
> 3. 在下拉菜单中**选择最新版本**
> 4. 点击安装

## 示例说明

### 基础轮询示例 (`basic_polling`)

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPad** → **basic_polling** 找到该示例。
- **示例说明**：通过蓝牙设备地址与手柄连接，实时查询、打印其所有按钮状态与摇杆数值。

### 输入状态检测示例 (`inputs_detection`)

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPad** → **inputs_detection** 找到该示例。
- **示例说明**：通过蓝牙设备地址与手柄连接，检测到按钮状态与摇杆数值变化后打印。

### 按键掩码连接示例 (`button_mask_connection`)

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPad** → **button_mask_connection** 找到该示例。
- **核心功能**：通过匹配特定的自定义的**按键**或者**按键组合**来扫描并自动连接附近的 CodexPad 设备，检测摇杆和按键变化并打印。
- **操作步骤**：代码启动后进入扫描连接状态，手柄开机后蓝灯闪烁，此时**按住**手柄上你代码中指定的按键掩码（按键组合）（默认为`Start` + `Cross(A)`）直到主机连接到手柄为止，之后正常操作手柄观察控制台的日志输出。

## 按键掩码连接功能详解

详情链接：[按键掩码连接功能详解](../../../codex_pad_guide/blob/main/button_mask_connection_explained.zh-CN.md#按键掩码连接功能详解)

## 安全提示

### 实时检测连接状态

💡 **建议在主循环中持续调用 `is_connected()` 以实时感知手柄连接状态。**

当检测到断连（如手柄断电、超出连接范围或蓝牙干扰）时，请**务必立即让受控设备停止动作**（例如小车刹车、机械臂锁定等）。若不及时处理，小车、机器人等运动类设备可能会因无法接收新的控制指令而保持原有状态，从而导致失控并引发安全隐患。

## API说明

详情链接：<https://codexpad.github.io/codex_pad_arduino_lib/html/zh-CN/annotated.html>

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。
