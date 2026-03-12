/**
 * @~English
 * @file inputs_detection.ino
 * @example inputs_detection.ino
 * @brief Demonstrates how to detect real-time button states and joystick movements of a connected CodexPad.
 * @details This example establishes a connection to a specific CodexPad device (by Bluetooth Device Address) and continuously monitors all user
 * inputs. It showcases the detection of three distinct button states: **pressed** (momentary press), **released** (momentary release), and
 * **holding** (sustained press). It also monitors the analog joystick axes and prints their values when a significant change beyond a set threshold
 * is detected, filtering out minor jitter.
 *          @note The `Update()` method must be called as frequently as possible within the main loop without delays to ensure real-time
 * responsiveness and prevent data packet loss.
 * @see CodexPad::Update
 * @see CodexPad::pressed
 * @see CodexPad::released
 * @see CodexPad::holding
 * @see CodexPad::HasAxisValueChanged
 * @see CodexPad::axis_value
 */
/**
 * @~Chinese
 * @file inputs_detection.ino
 * @example inputs_detection.ino
 * @brief 演示如何检测已连接的 CodexPad 设备的实时按钮状态与摇杆移动。
 * @details 本示例通过Bluetooth Device Address连接到指定的 CodexPad 设备，并持续监控所有用户输入。
 *          它展示了三种不同的按钮状态检测： **按下** (瞬间按下)、 **释放** (瞬间释放)和 **持续按住** 。
 *          同时，它监控模拟摇杆轴，当检测到超过设定阈值的显著变化时打印其值，从而过滤微小抖动。
 *          @note 必须在主循环中尽可能频繁地调用 `Update()` 方法，且不得添加延时，以确保实时响应性并防止数据包丢失。
 * @see CodexPad::Update
 * @see CodexPad::pressed
 * @see CodexPad::released
 * @see CodexPad::holding
 * @see CodexPad::HasAxisValueChanged
 * @see CodexPad::axis_value
 */

#include "codex_pad.h"

namespace {
// Replace with your CodexPad device's Bluetooth device address
// 替换为你的 CodexPad 的 Bluetooth device address
const std::string kBluetoothDeviceAddress = "E4:66:E5:A2:24:5D";

CodexPad g_codex_pad;

/**
 * Convert button constant to readable string name
 * 将按钮枚举转换为可读的字符串名称
 */
std::string ButtonToString(CodexPad::Button button) {
  switch (button) {
    case CodexPad::Button::kUp: {
      return "Up";  // 上按钮 | UP button
    }
    case CodexPad::Button::kDown: {
      return "Down";  // 下按钮 | DOWN button
    }
    case CodexPad::Button::kLeft: {
      return "Left";  // 左按钮 | LEFT button
    }
    case CodexPad::Button::kRight: {
      return "Right";  // 右按钮 | RIGHT button
    }
    case CodexPad::Button::kSquareX: {
      return "Square(X)";  // 方形 或者 X 按钮 | SQUARE or X button
    }
    case CodexPad::Button::kTriangleY: {
      return "Triangle(Y)";  // 三角 或者 Y 按钮 | TRIANGLE or Y button
    }
    case CodexPad::Button::kCrossA: {
      return "Cross(A)";  // 叉型 或者 A 按钮 | CROSS or A button
    }
    case CodexPad::Button::kCircleB: {
      return "Circle(B)";  // 圆形 或者 B 按钮 | CIRCLE or B button
    }
    case CodexPad::Button::kL1: {
      return "L1";  // L1按钮 | L1 button
    }
    case CodexPad::Button::kL2: {
      return "L2";  // L2按钮 | L2 button
    }
    case CodexPad::Button::kL3: {
      return "L3";  // L3按钮 | L3 button
    }
    case CodexPad::Button::kR1: {
      return "R1";  // R1按钮 | R1 button
    }
    case CodexPad::Button::kR2: {
      return "R2";  // R2按钮 | R2 button
    }
    case CodexPad::Button::kR3: {
      return "R3";  // R3按钮 | R3 button
    }
    case CodexPad::Button::kSelect: {
      return "Select";  // 选择按钮 | SELECT button
    }
    case CodexPad::Button::kStart: {
      return "Start";  // 开始按钮 | START button
    }
    case CodexPad::Button::kHome: {
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

  // Detect state changes for all buttons
  // Use pressed(), released(), holding() methods to detect different button states
  // 检测所有按钮的状态变化
  // 使用pressed(), released(), holding()方法检测按钮的不同状态
  for (auto button : {CodexPad::Button::kUp,
                      CodexPad::Button::kDown,
                      CodexPad::Button::kLeft,
                      CodexPad::Button::kRight,
                      CodexPad::Button::kSquareX,
                      CodexPad::Button::kTriangleY,
                      CodexPad::Button::kCrossA,
                      CodexPad::Button::kCircleB,
                      CodexPad::Button::kL1,
                      CodexPad::Button::kL2,
                      CodexPad::Button::kL3,
                      CodexPad::Button::kR1,
                      CodexPad::Button::kR2,
                      CodexPad::Button::kR3,
                      CodexPad::Button::kSelect,
                      CodexPad::Button::kStart,
                      CodexPad::Button::kHome}) {
    // Check if button was just pressed (transition from released to pressed)
    // 检测按钮是否刚刚按下（从弹起变为按下）
    if (g_codex_pad.pressed(button)) {
      printf("Button %s: pressed\n", ButtonToString(button).c_str());
    }

    // Check if button was just released (transition from pressed to released)
    // 检测按钮是否刚刚释放（从按下变为弹起）
    else if (g_codex_pad.released(button)) {
      printf("Button %s: released\n", ButtonToString(button).c_str());
    }

    // Check if button is holding
    // 检测按钮是否持续按下状态
    else if (g_codex_pad.holding(button)) {
      printf("Button %s: holding\n", ButtonToString(button).c_str());
    }
  }

  // Check if joystick axis values have changed significantly (using threshold to avoid minor jitter)
  // Threshold is set to 2, only consider changes equal to or greater than 2 units as significant
  // 检测摇杆轴值是否发生了有效变化（使用阈值避免微小抖动）
  // 阈值设置为2，只有当摇杆值变化达到或超过2个单位时才认为是有效变化
  constexpr uint8_t kAxisValueChangeThreshold = 2;

  // Check if left stick X or Y axis has significant change
  // 检测左摇杆X轴或Y轴是否有显著变化
  if (g_codex_pad.HasAxisValueChanged(CodexPad::Axis::kLeftStickX, kAxisValueChangeThreshold) ||
      g_codex_pad.HasAxisValueChanged(CodexPad::Axis::kLeftStickY, kAxisValueChangeThreshold) ||
      g_codex_pad.HasAxisValueChanged(CodexPad::Axis::kRightStickX, kAxisValueChangeThreshold) ||
      g_codex_pad.HasAxisValueChanged(CodexPad::Axis::kRightStickY, kAxisValueChangeThreshold)) {
    // Print current joystick axis values (0-255)
    // 打印摇杆轴的当前值（0-255）
    printf("L(X: %3" PRIu8 ", Y:%3" PRIu8 "), R(X: %3" PRIu8 ", Y: %3" PRIu8 ")\n",
           g_codex_pad.axis_value(CodexPad::Axis::kLeftStickX),   // 左摇杆X轴当前值 | Left stick X axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kLeftStickY),   // 左摇杆Y轴当前值 | Left stick Y axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kRightStickX),  // 右摇杆X轴当前值 | Right stick X axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kRightStickY)   // 右摇杆Y轴当前值 | Right stick Y axis current value
    );
  }
}