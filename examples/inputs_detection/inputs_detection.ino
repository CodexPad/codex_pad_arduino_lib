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
 * @see codex_pad::Client::Update
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
 * @see codex_pad::Client::Update
 */

#include <map>
#include <string>

#include "codex_pad.h"
#include "cyf.h"
#include "cyf/log.h"

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

const std::map<Button, std::string> kButtonNames{
    {Button::kUp, "Up"},
    {Button::kDown, "Down"},
    {Button::kLeft, "Left"},
    {Button::kRight, "Right"},
    {Button::kSquareX, "Square(X)"},
    {Button::kTriangleY, "Triangle(Y)"},
    {Button::kCrossA, "Cross(A)"},
    {Button::kCircleB, "Circle(B)"},
    {Button::kL1, "L1"},
    {Button::kR1, "R1"},
    {Button::kL2, "L2"},
    {Button::kR2, "R2"},
    {Button::kL3, "L3"},
    {Button::kR3, "R3"},
    {Button::kSelect, "Select"},
    {Button::kStart, "Start"},
    {Button::kHome, "Home"},
};

codex_pad::Client g_codex_pad_client;

void Connect() {
  CLOGI("Start to connect %s", kBluetoothDeviceAddress.c_str());
  // Connect to the CodexPad with specified Bluetooth device address
  // 连接到指定蓝牙设备地址的手柄
  while (!g_codex_pad_client.Connect(kBluetoothDeviceAddress, 5000)) {
    CLOGI("Retry to connect %s", kBluetoothDeviceAddress.c_str());
  }

  CLOGI("Remote device name: %s", g_codex_pad_client.remote_device_name().c_str());
  CLOGI("Remote model number: %s", g_codex_pad_client.remote_model_number().c_str());
  CLOGI("Remote firmware revision: %u.%u.%u", g_codex_pad_client.remote_firmware_version()[0],
        g_codex_pad_client.remote_firmware_version()[1], g_codex_pad_client.remote_firmware_version()[2]);

  if (const auto ble_client = g_codex_pad_client.ble_client(); ble_client != nullptr) {
    CLOGI("Remote Bluetooth Device Address: %s", ble_client->getPeerAddress().toString().c_str());
  } else {
    CLOGI("Remote Bluetooth Device Address: unknown");
  }

  CLOGI("Connected");
}

void RssiMonitor() {
  static uint64_t s_last_time = 0;

  if (s_last_time == 0 || millis() - s_last_time > 1000) {
    const int32_t rssi = g_codex_pad_client.rssi();
    if (rssi != 0) {
      if (rssi >= -65) {
        CLOGI("rssi: %" PRId32 " dBm, | [#][#][#][#] | strong", rssi);
      } else if (rssi >= -80) {
        CLOGI("rssi: %" PRId32 " dBm, | [#][#][#][ ] | decent", rssi);
      } else if (rssi >= -95) {
        CLOGW("rssi: %" PRId32 " dBm, | [#][#][ ][ ] | weak", rssi);
      } else {
        CLOGW("rssi: %" PRId32 " dBm, | [#][ ][ ][ ] | critical", rssi);
      }
    } else {
      CLOGI("rssi: unknown\n");
    }

    s_last_time = millis();
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);

  CLOGI("Init");
  g_codex_pad_client.Init();

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
  const gamepad::input::Tracker& it = g_codex_pad_client.Update();
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

  if (!g_codex_pad_client.is_connected()) {
    CLOGW("Disconnected, start to reconnect");
    Connect();
    return;
  }

  RssiMonitor();

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

  static std::map<Button, uint64_t> s_button_last_time;

  for (const auto& [button, name] : kButtonNames) {
    if (it.pressed(button)) {  // button was just pressed
      CLOGI("Button mask: 0x%08" PRIX32 ", button %s: pressed", it.raw().buttons, name.c_str());
      s_button_last_time[button] = millis();
    } else if (it.released(button)) {  // button was just released
      CLOGI("Button mask: 0x%08" PRIX32 ", button %s: released", it.raw().buttons, name.c_str());
      s_button_last_time[button] = millis();
    } else if (it.holding(button)) {  // button remains pressed
      if (s_button_last_time.find(button) == s_button_last_time.end() || s_button_last_time[button] == 0 ||
          millis() - s_button_last_time[button] > 100) {
        CLOGI("Button mask: 0x%08" PRIX32 ", button %s: holding", it.raw().buttons, name.c_str());
        s_button_last_time[button] = millis();
      }
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
  constexpr uint8_t kAxisValueChangeThreshold = 5;

  if (it.AxisChanged(Axis::kLeftStickX, kAxisValueChangeThreshold) ||
      it.AxisChanged(Axis::kLeftStickY, kAxisValueChangeThreshold) ||
      it.AxisChanged(Axis::kRightStickX, kAxisValueChangeThreshold) ||
      it.AxisChanged(Axis::kRightStickY, kAxisValueChangeThreshold)) {
    CLOGI("L(X: %3" PRIu8 ", Y:%3" PRIu8 "), R(X: %3" PRIu8 ", Y: %3" PRIu8 ")", it[Axis::kLeftStickX], it[Axis::kLeftStickY],
          it[Axis::kRightStickX], it[Axis::kRightStickY]);
  }
}