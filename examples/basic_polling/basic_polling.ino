/**
 * @~English
 * @file basic_polling.ino
 * @example basic_polling.ino
 * @brief Demonstrates the basic polling method to periodically query and print all CodexPad button states and joystick values.
 * @details This example establishes a connection to a specific CodexPad device (by Bluetooth Device Address) and implements a
 *          simple polling loop. Every 30 milliseconds, it queries and prints the current state (pressed/released) of all
 *          buttons and the raw analog values (0-255) of both joysticks. It showcases the fundamental usage of
 *          `gamepad::input::Tracker[Button]` for discrete button queries and `gamepad::input::Tracker[Axis]` for continuous
 *          joystick readings.
 * @note This example uses a simple timing mechanism (`millis()`) to print at a fixed interval, which is suitable for
 *       monitoring or logging. For real-time control, ensure `Update()` is called as frequently as possible without blocking
 *       delays.
 * @see CodexPad::Update
 */
/**
 * @~Chinese
 * @file basic_polling.ino
 * @example basic_polling.ino
 * @brief 演示通过基本轮询方式定期查询并打印 CodexPad 所有按钮状态与摇杆值。
 * @details 本示例通过Bluetooth Device Address连接到指定的 CodexPad 设备，并实现了一个简单的轮询循环。
 *          每隔 30 毫秒，它会查询并打印所有按钮的当前状态（按下/弹起）以及两个摇杆的原始模拟值（0-255）。
 *          它展示了 `gamepad::input::Tracker[Button]` 用于离散按钮查询和 `gamepad::input::Tracker[Axis]`
 *          用于连续摇杆读取的基本用法。
 * @note 本示例使用简单的定时机制（`millis()`）以固定间隔打印，适用于状态监控或日志记录。
 *       对于实时控制应用，请确保尽可能频繁地调用 `Update()` 且无阻塞延时。
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
using namespace gamepad::input;  // ⚠️ DO NOT REMOVE THIS LINE ⚠️

namespace {
// Replace with your CodexPad device's Bluetooth device address
// 替换为你的 CodexPad 的 Bluetooth device address
const std::string kBluetoothDeviceAddress = "16:00:00:00:03:27";

CodexPad g_codex_pad;

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

  // Set transmission power to 0 dBm
  // Transmission power affects communication range and power consumption:
  // Higher power provides longer range but consumes more battery
  // Choose appropriate power level based on your application to balance range and battery life
  // 设置发射功率为 0 dBm
  // 发射功率影响通信距离和功耗：功率越高，通信距离越远，但功耗也越大
  // 建议根据实际应用场景选择合适的功率等级以平衡距离和电池寿命
  if (g_codex_pad.set_remote_tx_power(CodexPad::TxPower::k0dBm)) {
    printf("Set remote tx power to 0 dBm successfully\n");
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

  if (!g_codex_pad.is_connected()) {
    printf("Disconnected, start to reconnect\n");
    Connect();
    return;
  }

  static uint32_t s_print_time = 0;
  if (s_print_time != 0 && s_print_time + 30 > millis()) {
    return;
  }

  s_print_time = millis();

  printf(
      "Up:%u, Down:%u, Left:%u, Right:%u, Square(X):%u, Triangle(Y):%u, Cross(A):%u, Circle(B):%u, L1:%u, L2:%u, L3:%u, R1:%u, "
      "R2:%u, R3:%u, Select:%u, Start:%u, Home:%u, L(X:%3u, Y:%3u), R(X:%3u, Y:%3u)\n",

      // ------------------------------------------------------------------
      // Button states (boolean -> uint)
      // operator[] returns bool:
      //   true  = pressed
      //   false = released
      //
      // 按钮状态（bool 类型，打印时被隐式转换为 1 / 0）
      // true  : 按下
      // false : 弹起
      // ------------------------------------------------------------------
      it[Button::kUp], it[Button::kDown], it[Button::kLeft], it[Button::kRight], it[Button::kSquareX], it[Button::kTriangleY],
      it[Button::kCrossA], it[Button::kCircleB], it[Button::kL1], it[Button::kL2], it[Button::kL3], it[Button::kR1],
      it[Button::kR2], it[Button::kR3], it[Button::kSelect], it[Button::kStart], it[Button::kHome],

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
      it[Axis::kLeftStickX], it[Axis::kLeftStickY], it[Axis::kRightStickX], it[Axis::kRightStickY]);
}