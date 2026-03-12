/**
 * @~English
 * @file basic_polling.ino
 * @example basic_polling.ino
 * @brief Demonstrates the basic polling method to periodically query and print all CodexPad button states and joystick values.
 * @details This example establishes a connection to a specific CodexPad device (by Bluetooth Device Address) and implements a simple polling loop.
 *          Every 30 milliseconds, it queries and prints the current state (pressed/released) of all buttons and the raw analog values (0-255) of both
 * joysticks. It showcases the fundamental usage of `button_state()` for discrete button queries and `axis_value()` for continuous joystick readings.
 *          @note This example uses a simple timing mechanism (`millis()`) to print at a fixed interval, which is suitable for monitoring or logging.
 *          For real-time control, ensure `Update()` is called as frequently as possible without blocking delays.
 * @see CodexPad::button_state
 * @see CodexPad::axis_value
 * @see CodexPad::Update
 */
/**
 * @~Chinese
 * @file basic_polling.ino
 * @example basic_polling.ino
 * @brief 演示通过基本轮询方式定期查询并打印 CodexPad 所有按钮状态与摇杆值。
 * @details 本示例通过Bluetooth Device Address连接到指定的 CodexPad 设备，并实现了一个简单的轮询循环。
 *          每隔 30 毫秒，它会查询并打印所有按钮的当前状态（按下/弹起）以及两个摇杆的原始模拟值（0-255）。
 *          它展示了 `button_state()` 用于离散按钮查询和 `axis_value()` 用于连续摇杆读取的基本用法。
 *          @note 本示例使用简单的定时机制（`millis()`）以固定间隔打印，适用于状态监控或日志记录。
 *          对于实时控制应用，请确保尽可能频繁地调用 `Update()` 且无阻塞延时。
 * @see CodexPad::button_state
 * @see CodexPad::axis_value
 * @see CodexPad::Update
 */

#include "codex_pad.h"

namespace {
// Replace with your CodexPad device's Bluetooth device address
// 替换为你的 CodexPad 的 Bluetooth device address
const std::string kBluetoothDeviceAddress = "E4:66:E5:A2:24:5D";

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
  printf("Remote firmware revision: %u.%u.%u\n",
         g_codex_pad.remote_firmware_version()[0],
         g_codex_pad.remote_firmware_version()[1],
         g_codex_pad.remote_firmware_version()[2]);

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
  // Important: Update() method must be called as frequently as possible in the loop, no delays should be added
  // This method processes all received Bluetooth packets, delays will cause data loss and response lag
  // For real-time control applications, high-frequency calls are essential to ensure prompt response to gamepad input
  // 重要：Update()方法必须在循环中尽可能频繁地调用，不能添加延时
  // 该方法负责处理所有接收到的蓝牙数据包，延时会导致数据丢失和响应延迟
  // 对于实时控制应用，必须保持高频率调用以确保及时响应手柄输入
  g_codex_pad.Update();

  if (!g_codex_pad.is_connected()) {
    printf("Disconnected, start to reconnect\n");
    Connect();
    return;
  }

  static uint32_t s_print_time = 0;
  if (s_print_time == 0 || s_print_time + 30 < millis()) {
    s_print_time = millis();

    printf(
        "Up:%u, Down:%u, Left:%u, Right:%u, Square(X):%u, Triangle(Y):%u, Cross(A):%u, Circle(B):%u, L1:%u, L2:%u, L3:%u, R1:%u, R2:%u, "
        "R3:%u, Select:%u, "
        "Start:%u, Home:%u, L(X:%3u, Y:%3u), R(X:%3u, Y:%3u)\n",

        // Get button states, button_state() returns bool type, true means pressed, false means released
        // 获取各个按钮的状态，button_state()返回bool类型，true表示按下，false表示弹起
        g_codex_pad.button_state(CodexPad::Button::kUp),
        g_codex_pad.button_state(CodexPad::Button::kDown),
        g_codex_pad.button_state(CodexPad::Button::kLeft),
        g_codex_pad.button_state(CodexPad::Button::kRight),
        g_codex_pad.button_state(CodexPad::Button::kSquareX),
        g_codex_pad.button_state(CodexPad::Button::kTriangleY),
        g_codex_pad.button_state(CodexPad::Button::kCrossA),
        g_codex_pad.button_state(CodexPad::Button::kCircleB),
        g_codex_pad.button_state(CodexPad::Button::kL1),
        g_codex_pad.button_state(CodexPad::Button::kL2),
        g_codex_pad.button_state(CodexPad::Button::kL3),
        g_codex_pad.button_state(CodexPad::Button::kR1),
        g_codex_pad.button_state(CodexPad::Button::kR2),
        g_codex_pad.button_state(CodexPad::Button::kR3),
        g_codex_pad.button_state(CodexPad::Button::kSelect),
        g_codex_pad.button_state(CodexPad::Button::kStart),
        g_codex_pad.button_state(CodexPad::Button::kHome),

        // Get joystick axis data, axis_value() returns value from 0 to 255
        // Center position is around 128, values represent stick deflection
        // 获取摇杆轴数据，axis_value()返回0~255的数值
        // 中间位置约为128，数值范围表示摇杆的偏移程度
        g_codex_pad.axis_value(CodexPad::Axis::kLeftStickX),
        g_codex_pad.axis_value(CodexPad::Axis::kLeftStickY),
        g_codex_pad.axis_value(CodexPad::Axis::kRightStickX),
        g_codex_pad.axis_value(CodexPad::Axis::kRightStickY));
  }
}