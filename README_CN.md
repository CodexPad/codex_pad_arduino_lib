# 🎮 CodexPad Arduino 库

[English](README.md)

## 概述

本库为**CodexPad**系列手柄提供的 **Arduino 平台专用库**，支持ESP32系列开发板通过蓝牙连接并读取CodexPad手柄的所有按键与摇杆输入状态。关于 CodexPad 产品的详细信息，请查阅以下产品文档。

| CodexPad型号 | 链接 |
| :--- | :--- |
| CodexPad-C10 | <https://github.com/CodexPad/codex_pad_c10> |
| CodexPad-S10 | <https://github.com/CodexPad/codex_pad_s10> |

## 支持的硬件平台

| 支持的硬件平台 |
| :--- |
| ESP32 |
| ESP32-S2 |
| ESP32-S3 |
| ESP32-C3 |
| ESP32-C5 |
| ESP32-C6 |
| ESP32-H2 |
| ESP32-P4 |

## 特性

- **灵活的双模式连接**：

  - **Bluetooth Device Address直连**：通过已知的**Bluetooth Device Address**，快速与指定手柄建立稳定连接。

  - **按键掩码扫描连接**：无需提前知道**Bluetooth Device Address**。通过扫描并匹配目标手柄上被按住的、由用户代码自定义的按键组合（即“按钮掩码”），自动连接信号最强（RSSI最大）的设备，实现快速、灵活的配对。

- **实时按键事件检测**：可实时读取所有按键的输入状态，并区分**按下**、**释放**和**长按**三种事件。

- **高精度摇杆数据**：获取左右摇杆X轴和Y轴的模拟量数值，范围从0至255，提供精准的控制输入。

- **可调发射功率**：允许根据实际应用场景（如距离、功耗需求），在 **-16 dBm** 至 **+6 dBm** 范围内动态调整蓝牙发射功率。

## 使用说明

### 准备工作

在开始编程前，请完成以下准备工作，以确保开发过程顺利进行。

#### 熟悉产品文档

- 详细阅读 CodexPad 产品手册，全面了解硬件特性、熟悉手柄按键摇杆布局、功能定义、指示灯状态以及开关机操作等基本信息。

#### 获取并记录手柄**Bluetooth Device Address(BD_ADDR)**

> **⚠️ 重要提示**：本库直连的示例是通过 **Bluetooth Device Address(BD_ADDR)** 进行连接。**编程时，必须在代码明确指定您手柄的Bluetooth Device Address(BD_ADDR)。**

请参考产品手册中提供的方法，获取您手柄的**Bluetooth Device Address(BD_ADDR)**。其格式通常为 `"E4:66:E5:A2:24:5D"`（由0-9、A-F的字符组成，冒号为半角）。请妥善记录此信息，后续需要在代码为您自己手柄的实际**Bluetooth Device Address(BD_ADDR)**。

#### 开启手柄并进入待连接状态

- 将手柄开机，手柄开机后会自动处于蓝牙可被发现的**待连接状态**，此时手柄指示灯应呈现**慢闪状态（约每秒闪烁一次）**。

### 安装 ESP32 开发板管理器

1. 在 Arduino IDE 中，打开**工具** > **开发板** > **开发板管理器...**。
2. 在搜索框中输入 `esp32`，找到并安装 **ESP32 by Espressif Systems**。推荐选择 **3.3.7** 或 **3.3.7-cn** 版本。
3. 安装完成后，在**工具** > **开发板**列表中选择您使用的具体 ESP32 开发板型号（如 `ESP32 Dev Module`）。
4. 通过 USB 数据线将开发板连接至电脑，并在**工具** > **端口**菜单中选择正确的串行端口。

### 安装必要的依赖库

> **⚠️ 重要提示**： 本库依赖于 **NimBLE-Arduino** 库来实现蓝牙功能。在安装和使用 CodexPad 库之前，**必须先完成此依赖库的安装**。

- **库名称**：NimBLE-Arduino
- **最低版本要求**：>= 2.3.9
- **安装方法**：
    1. 打开 Arduino IDE。
    2. 点击 **工具** -> **管理库...**，打开库管理器。
    3. 在搜索框中输入 **`NimBLE-Arduino`**。
    4. 在搜索结果中找到由 **`h2zero`** 维护的 **`NimBLE-Arduino`** 库（**NimBLE-Arduino** by h2zero）（**通常它会是搜索结果中的第一个**）。
    5. 点击“安装”按钮，选择不低于 **`2.3.9`** 的最新版本进行安装。

### 安装 CodexPad 库

- 前往GitHub仓库Release（<https://github.com/CodexPad/codex_pad_arduino_lib/releases>）面进行下载
- 在 Arduino IDE 中，通过**项目** → **加载库** → **添加 .ZIP 库...** 来安装此文件。

## 示例说明

### 基础轮询示例 (`basic_polling`)

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPad** → **basic_polling** 找到该示例。
- **示例说明**：通过Bluetooth Device Address与CodexPad蓝牙连接，实时查询、打印其所有按钮状态与摇杆数值。

### 输入状态检测示例 (`inputs_detection`)

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPad** → **inputs_detection** 找到该示例。
- **示例说明**：通过Bluetooth Device Address与CodexPad蓝牙连接，检测到按钮状态与摇杆数值变化后打印。

### 扫描连接示例 (`scan_and_connect`)

- **示例位置**：在 Arduino IDE 中，通过 **文件** → **示例** → **CodexPad** → **scan_and_connect** 找到该示例。
- **核心功能**：通过匹配特定的自定义的**按键**或者**按键组合**来扫描并自动连接附近的 CodexPad 设备，检测摇杆和按键变化并打印。
- **重要警告**：按钮掩码中**绝对禁止**包含 `Home` 键，因为长按 Home 键会导致手柄重启，从而中断连接。

## API说明

详情链接：<https://codexpad.github.io/codex_pad_arduino_lib/html/zh-CN/annotated.html>

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。
