/**
 * @~English
 * @file basic_polling.ino
 * @example basic_polling.ino
 * @brief Demonstrates the basic polling method to periodically query and print all CodexPad button states and joystick values.
 * @details This example establishes a connection to a specific CodexPad device (by Bluetooth Device Address) and implements a
 *          simple polling loop. Every 50 milliseconds, it queries and prints the current state (pressed/released) of all
 *          buttons and the raw analog values (0-255) of both joysticks. It showcases the fundamental usage of
 *          `gamepad::input::Tracker[Button]` for discrete button queries and `gamepad::input::Tracker[Axis]` for continuous
 *          joystick readings.
 * @note This example uses a simple timing mechanism (`millis()`) to print at a fixed interval, which is suitable for
 *       monitoring or logging. For real-time control, ensure `Update()` is called as frequently as possible without blocking
 *       delays.
 * @see codex_pad::Client::Update
 */
/**
 * @~Chinese
 * @file basic_polling.ino
 * @example basic_polling.ino
 * @brief 演示通过基本轮询方式定期查询并打印手柄所有按钮状态与摇杆值。
 * @details 本示例通过地址连接到指定的手柄，并实现了一个简单的轮询循环。
 *          每隔 50 毫秒，它会查询并打印所有按钮的当前状态（按下/弹起）以及两个摇杆的原始模拟值（0-255）。
 *          它展示了 `gamepad::input::Tracker[Button]` 用于离散按钮查询和 `gamepad::input::Tracker[Axis]`
 *          用于连续摇杆读取的基本用法。
 * @note 本示例使用简单的定时机制（`millis()`）以固定间隔打印，适用于状态监控或日志记录。
 *       对于实时控制应用，请确保尽可能频繁地调用 `Update()` 且无阻塞延时。
 * @see codex_pad::Client::Update
 */

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
using namespace gamepad::input;  // ⚠️ DO NOT REMOVE THIS LINE ⚠️

namespace {
// Replace with your CodexPad device's Bluetooth device address
// 替换为你的 CodexPad 的 Bluetooth device address
const std::string kBluetoothDeviceAddress = "E4:66:E5:A2:17:06";

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
}  // namespace

void setup() {
  Serial.begin(115200);

  CLOGI("Init");

  // Set the TX power for the local BLE device (e.g., ESP32) in dBm.
  // 9dBm is the maximum TX power for ESP32 in BLE mode, which helps improve signal coverage.
  // NimBLETxPowerType::All applies this power level to both advertising and connection channels.
  const int8_t local_tx_power = 9;  // Local device TX power in dBm
  g_codex_pad_client.Init(local_tx_power);

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
  // Tracker: Access button and joystick states
  // ==========================================================================
  // • Returned Tracker provides access to ALL gamepad inputs
  // • Buttons  → it[Button::kXXX]  (bool, true = pressed)
  // • Axes     → it[Axis::kXXX]    (uint8_t, 0–255)
  //
  // 📚 GamepadInput Library Documentation:
  // https://codexpad.github.io/gamepad_input_arduino_lib/
  //
  // • Tracker 可用于访问所有按钮和摇杆状态
  // • 按钮   → it[Button::kXXX]（bool，true 表示按下）
  // • 摇杆轴 → it[Axis::kXXX]（0～255）
  //
  // 📘 库文档：
  // https://codexpad.github.io/gamepad_input_arduino_lib/
  // ==========================================================================

  if (!g_codex_pad_client.is_connected()) {
    CLOGW("Disconnected, start to reconnect\n");
    Connect();
    return;
  }

  static uint32_t s_print_time = 0;
  if (s_print_time != 0 && s_print_time + 50 > millis()) {
    return;
  }

  s_print_time = millis();

  std::string log;

  char buffer[512] = {0};
  size_t length = snprintf(buffer, sizeof(buffer), "Button mask: 0x%08" PRIX32, static_cast<uint32_t>(it.raw().buttons));
  log.append(buffer, length);

  // ------------------------------------------------------------------
  // Joystick axis values (0–255)
  // Center position ≈ 128
  // Smaller → left / down
  // Larger  → right / up
  //
  // 摇杆轴数据（0～255）
  // 中间值约 128
  // 越小越左 / 下，越大越右 / 上
  // ------------------------------------------------------------------
  length = snprintf(buffer, sizeof(buffer), ", L(%3u, %3u), R(%3u, %3u)", it[Axis::kLeftStickX], it[Axis::kLeftStickY],
                    it[Axis::kRightStickX], it[Axis::kRightStickY]);
  log.append(buffer, length);

  // ------------------------------------------------------------------
  // Button states (boolean -> uint)
  // operator[] returns bool:
  //   true  = pressed
  //   false = released
  //
  // 按钮状态（bool 类型）
  // true  : 按下
  // false : 弹起
  if (it[Button::kUp]) {
    log.append(", Up");
  }

  if (it[Button::kDown]) {
    log.append(", Down");
  }

  if (it[Button::kLeft]) {
    log.append(", Left");
  }

  if (it[Button::kRight]) {
    log.append(", Right");
  }

  if (it[Button::kSquareX]) {
    log.append(", Square(X)");
  }

  if (it[Button::kTriangleY]) {
    log.append(", Triangle(Y)");
  }

  if (it[Button::kCrossA]) {
    log.append(", Cross(A)");
  }

  if (it[Button::kCircleB]) {
    log.append(", Circle(B)");
  }

  if (it[Button::kL1]) {
    log.append(", L1");
  }

  if (it[Button::kL2]) {
    log.append(", L2");
  }

  if (it[Button::kL3]) {
    log.append(", L3");
  }

  if (it[Button::kR1]) {
    log.append(", R1");
  }

  if (it[Button::kR2]) {
    log.append(", R2");
  }

  if (it[Button::kR3]) {
    log.append(", R3");
  }

  if (it[Button::kSelect]) {
    log.append(", Select");
  }

  if (it[Button::kStart]) {
    log.append(", Start");
  }

  if (it[Button::kHome]) {
    log.append(", Home");
  }

  CLOGI("%s", log.c_str());
}