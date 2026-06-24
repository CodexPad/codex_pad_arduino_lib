/**
 * @~English
 * @file inputs_detection.ino
 * @example inputs_detection.ino
 * @brief Demonstrates how to detect real-time button states and joystick movements of a connected CodexPad.
 * @details This example establishes a connection to a specific CodexPad device (by Bluetooth Device Address) and continuously
 *          monitors all user inputs. It showcases the detection of three distinct button states: **pressed** (momentary press),
 *          **released** (momentary release), and
 *          **holding** (sustained press). It also monitors the analog joystick axes and prints their values when a significant
 *          change beyond a set threshold is detected, filtering out minor jitter.
 * @note The `Update()` method must be called as frequently as possible within the main loop without delays to ensure
 *       real-time responsiveness and prevent data packet loss.
 * @see CodexPad::Update
 */
/**
 * @~Chinese
 * @file inputs_detection.ino
 * @example inputs_detection.ino
 * @brief 演示如何检测已连接的 CodexPad 设备的实时按钮状态与摇杆移动。
 * @details 本示例通过Bluetooth Device Address连接到指定的 CodexPad 设备，并持续监控所有用户输入。
 *          它展示了三种不同的按钮状态检测： **按下** (瞬间按下)、 **释放** (瞬间释放)和 **持续按住** 。
 *          同时，它监控模拟摇杆轴，当检测到超过设定阈值的显著变化时打印其值，从而过滤微小抖动。
 * @note 必须在主循环中尽可能频繁地调用 `Update()` 方法，且不得添加延时，以确保实时响应性并防止数据包丢失。
 * @see CodexPad::Update
 */

#include "codex_pad.h"

/**
 * IMPORTANT:
 * This using directive is REQUIRED to directly access `Button` and `Axis`.
 * Without it, you must write the fully qualified names:
 *   gamepad::input::Button::kUp
 *   gamepad::input::Axis::kLeftStickX
 * Forgetting this line will cause compile errors when using Button or Axis.
 */
/**
 * 重要：
 * 必须使用该命名空间，否则无法直接访问 `Button` 和 `Axis`。
 * 如果没有这一行，就必须写成完整限定名：
 *   gamepad::input::Button::kUp
 *   gamepad::input::Axis::kLeftStickX
 * 忘记引入命名空间会导致编译失败。
 */
using namespace gamepad::input;

namespace {
// Replace with your CodexPad device's Bluetooth device address
// 替换为你的 CodexPad 的 Bluetooth device address
const std::string kBluetoothDeviceAddress = "E4:66:E5:A2:17:06";

CodexPad g_codex_pad;

/**
 * Convert button constant to readable string name
 * 将按钮枚举转换为可读的字符串名称
 */
std::string ButtonToString(Button button) {
  switch (button) {
    case Button::kUp: {
      return "Up";  // 上按钮 | UP button
    }
    case Button::kDown: {
      return "Down";  // 下按钮 | DOWN button
    }
    case Button::kLeft: {
      return "Left";  // 左按钮 | LEFT button
    }
    case Button::kRight: {
      return "Right";  // 右按钮 | RIGHT button
    }
    case Button::kSquareX: {
      return "Square(X)";  // 方形 或者 X 按钮 | SQUARE or X button
    }
    case Button::kTriangleY: {
      return "Triangle(Y)";  // 三角 或者 Y 按钮 | TRIANGLE or Y button
    }
    case Button::kCrossA: {
      return "Cross(A)";  // 叉型 或者 A 按钮 | CROSS or A button
    }
    case Button::kCircleB: {
      return "Circle(B)";  // 圆形 或者 B 按钮 | CIRCLE or B button
    }
    case Button::kL1: {
      return "L1";  // L1按钮 | L1 button
    }
    case Button::kL2: {
      return "L2";  // L2按钮 | L2 button
    }
    case Button::kL3: {
      return "L3";  // L3按钮 | L3 button
    }
    case Button::kR1: {
      return "R1";  // R1按钮 | R1 button
    }
    case Button::kR2: {
      return "R2";  // R2按钮 | R2 button
    }
    case Button::kR3: {
      return "R3";  // R3按钮 | R3 button
    }
    case Button::kSelect: {
      return "Select";  // 选择按钮 | SELECT button
    }
    case Button::kStart: {
      return "Start";  // 开始按钮 | START button
    }
    case Button::kHome: {
      return "Home";  // 首页按钮 | HOME button
    }
    default: {
      return {};  // 未知按钮返回空字符串 | Unknown button returns empty string
    }
  }
}

void Connect() {
  printf("Start to connect %s\n", kBluetoothDeviceAddress.c_str());
  // Connect to the CodexPad with specified Bluetooth device address
  // 连接到指定蓝牙设备地址的手柄
  while (!g_codex_pad.Connect(kBluetoothDeviceAddress, 5000)) {
    printf("Retry to connect %s\n", kBluetoothDeviceAddress.c_str());
  }

  printf("Remote device name: %s\n", g_codex_pad.remote_device_name().c_str());
  printf("Remote model number: %s\n", g_codex_pad.remote_model_number().c_str());
  printf("Remote firmware revision: %u.%u.%u\n", g_codex_pad.remote_firmware_version()[0],
         g_codex_pad.remote_firmware_version()[1], g_codex_pad.remote_firmware_version()[2]);

  if (const auto ble_client = g_codex_pad.ble_client(); ble_client != nullptr) {
    printf("Remote Bluetooth Device Address: %s\n", ble_client->getPeerAddress().toString().c_str());
  } else {
    printf("Remote Bluetooth Device Address: unknown\n");
  }

  // Set transmission power to 0dBm
  // Transmission power affects communication range and power consumption:
  // Higher power provides longer range but consumes more battery
  // Choose appropriate power level based on your application to balance range and battery life
  // 设置发射功率为0dBm
  // 发射功率影响通信距离和功耗：功率越高，通信距离越远，但功耗也越大
  // 建议根据实际应用场景选择合适的功率等级以平衡距离和电池寿命
  if (g_codex_pad.set_remote_tx_power(CodexPad::TxPower::k0dBm)) {
    printf("Set remote tx power to 0dBm successfully\n");
  }

  printf("Connected\n");
}
}  // namespace

void setup() {
  Serial.begin(115200);

  printf("Init\n");
  g_codex_pad.Init();

  Connect();
}

void loop() {
  // ==========================================================================
  // 🔴 CRITICAL: Call Update() as frequently as possible in loop()
  // ==========================================================================
  // • Update() processes incoming Bluetooth packets from the CodexPad
  // • Any delay(...) or long blocking code WILL cause:
  //     - Packet loss
  //     - Input lag
  //     - Unstable connection
  //
  // • For real-time control, call Update() every loop iteration
  //   without any blocking operations
  //
  // 🔴【重要】Update() 必须在 loop() 中尽可能高频调用
  // • Update() 负责处理来自 CodexPad 的蓝牙数据包
  // • 任何形式的 delay 或阻塞代码都会导致：
  //     - 数据丢失
  //     - 响应延迟
  //     - 连接不稳定
  //
  // • 实时控制应用中，必须每轮循环都调用 Update()，不可阻塞
  // ==========================================================================
  const gamepad::input::Tracker& it = g_codex_pad.Update();
  // ==========================================================================
  // Tracker: Gamepad Input Snapshot & Change Engine
  // ==========================================================================
  // • Returned by Update()
  // • Maintains previous and current input snapshots
  // • Enables edge detection and delta detection
  //
  // Tracker 由 Update() 返回
  // 内部保存上一帧和当前帧的输入数据
  // 支持边沿检测和差值检测
  //
  // 📚 https://codexpad.github.io/gamepad_input_arduino_lib/
  // ==========================================================================

  if (!g_codex_pad.is_connected()) {
    printf("Disconnected, start to reconnect\n");
    Connect();
    return;
  }

  // ==========================================================================
  // 🟢 Button State Change Detection (Edge-Based)
  // ==========================================================================
  // • pressed()  → button was just pressed (released → pressed)
  // • released() → button was just released (pressed → released)
  // • holding()  → button remains pressed across frames
  //
  // These APIs are *frame-differential* and rely on Update() frequency.
  // Ideal for UI navigation, action triggers, and avoiding repeat firing.
  //
  // • pressed()  → 按钮刚被按下（弹起 → 按下）
  // • released() → 按钮刚被释放（按下 → 弹起）
  // • holding()  → 按钮在两帧之间持续按下
  //
  // 这些接口是“帧间差分”的，依赖于 Update() 的高频调用
  // 非常适合 UI 导航、动作触发和防止长按连发
  // ==========================================================================
  for (auto button : {Button::kUp, Button::kDown, Button::kLeft, Button::kRight, Button::kSquareX, Button::kTriangleY,
                      Button::kCrossA, Button::kCircleB, Button::kL1, Button::kL2, Button::kL3, Button::kR1, Button::kR2,
                      Button::kR3, Button::kSelect, Button::kStart, Button::kHome}) {
    if (it.pressed(button)) {
      printf("Button %s: pressed\n", ButtonToString(button).c_str());
    } else if (it.released(button)) {
      printf("Button %s: released\n", ButtonToString(button).c_str());
    } else if (it.holding(button)) {
      printf("Button %s: holding\n", ButtonToString(button).c_str());
    }
  }

  // ==========================================================================
  // 🟢 Joystick Axis Change Detection (Threshold-Based Filtering)
  // ==========================================================================
  // • AxisChanged() detects significant changes between frames
  // • Uses a threshold to filter out noise and minor jitter
  // • Only reports movement when change ≥ threshold
  //
  // • AxisChanged() 用于检测摇杆轴值的有效变化
  // • 使用阈值过滤微小抖动和噪声
  // • 只有当变化幅度 ≥ 阈值时才视为有效移动
  // ==========================================================================
  constexpr uint8_t kAxisValueChangeThreshold = 2;

  if (it.AxisChanged(Axis::kLeftStickX, kAxisValueChangeThreshold) ||
      it.AxisChanged(Axis::kLeftStickY, kAxisValueChangeThreshold) ||
      it.AxisChanged(Axis::kRightStickX, kAxisValueChangeThreshold) ||
      it.AxisChanged(Axis::kRightStickY, kAxisValueChangeThreshold)) {
    printf("L(X: %3" PRIu8 ", Y:%3" PRIu8 "), R(X: %3" PRIu8 ", Y: %3" PRIu8 ")\n", it[Axis::kLeftStickX],
           it[Axis::kLeftStickY], it[Axis::kRightStickX], it[Axis::kRightStickY]);
  }
}