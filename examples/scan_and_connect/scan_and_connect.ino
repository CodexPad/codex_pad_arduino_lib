/**
 * @~English
 * @file scan_and_connect.ino
 * @example scan_and_connect.ino
 * @brief Demonstrates how to scan for and connect to a CodexPad device by matching specific button presses.
 * @details This example shows the usage of the `ScanAndConnect()` function. The device will scan for nearby CodexPad devices and automatically
 * connect to one where the operator is holding down the predefined combination of buttons (the *button mask*). The button mask is defined by the
 * `kExpectedButtonMask` constant. You must physically press and hold the corresponding button(s) on the target CodexPad for the connection to
 * succeed.
 * @warning **Important:** The button mask must NOT include `Button::kHome`. Pressing and holding the Home button causes the device to
 * reboot, which will interrupt or prevent connection.
 * @see CodexPad::ScanAndConnect
 * @see CodexPad::ButtonMask
 */
/**
 * @~Chinese
 * @file scan_and_connect.ino
 * @example scan_and_connect.ino
 * @brief 演示如何通过匹配特定按键按压来扫描并连接 CodexPad 设备。
 * @details 本示例展示了 `ScanAndConnect()` 函数的使用方法。设备将扫描附近的 CodexPad
 * 设备，并自动连接到操作者正按住预定义按键组合（*按钮掩码*）的那一个。 按钮掩码由常量 `kExpectedButtonMask` 定义。您必须在目标 CodexPad
 * 手柄上物理按住对应的按键，连接才能成功。
 * @warning **重要：** 按钮掩码中 **不得** 包含 `Button::kHome`。按住 Home 键会导致设备重启，从而中断或阻止连接。
 * @see CodexPad::ScanAndConnect
 * @see CodexPad::ButtonMask
 */

#include "codex_pad.h"

namespace {
// You can set a button mask to automatically connect when a target device is scanned and its button state matches this mask.
// For example, you can set it to connect only when a specific button is pressed, or when multiple specified buttons are pressed simultaneously on the
// device.
// 你可以设置一个按钮掩码，当扫描到目标设备并检测到其按键状态与该掩码匹配时，自动进行连接。
// 例如，可以设置为当设备上某个特定按键被按下，或多个指定按键被同时按下时才建立连接。

// 【Important Warning】DO NOT use `Button::kHome` (Home button) to set the button mask. Pressing and holding the Home button will trigger a device
// reboot, which will interrupt the connection process or put the device into an unexpected state.
// 【重要警告】请勿使用 `Button::kHome` (Home键) 来设置按钮掩码。因为按住Home键会触发设备重启，这将导致连接过程中断或设备进入不可预期的状态。

// Example: The button mask to match - Only the Start button
// 示例：需要匹配的按钮掩码 - 仅Start按钮
// constexpr auto kExpectedButtonMask = CodexPad::ButtonMask(CodexPad::Button::kStart);

// Example: The button mask to match - Start and CrossA buttons
// 示例：需要匹配的按钮掩码 - Start 和 CrossA 按钮
constexpr auto kExpectedButtonMask = CodexPad::ButtonMask(CodexPad::Button::kStart, CodexPad::Button::kCrossA);

// Example: The button mask to match - Start, CrossA, and SquareX buttons
// 示例：需要匹配的按钮掩码 - Start、CrossA 和 SquareX 按钮
// constexpr auto kExpectedButtonMask = CodexPad::ButtonMask(CodexPad::Button::kStart, CodexPad::Button::kCrossA, CodexPad::Button::kSquareX);

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
  printf("Start to scan and connect, button mask: 0x%08X\n", kExpectedButtonMask);

  while (!g_codex_pad.ScanAndConnect(kExpectedButtonMask)) {
    printf("Retry to scan and connect, button mask: 0x%08X\n", kExpectedButtonMask);
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
  for (const auto button : {CodexPad::Button::kUp,
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
    printf("L(X: %3" PRIu8 ", Y: %3" PRIu8 "), R(X: %3" PRIu8 ", Y: %3" PRIu8 ")\n",
           g_codex_pad.axis_value(CodexPad::Axis::kLeftStickX),   // 左摇杆X轴当前值 | Left stick X axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kLeftStickY),   // 左摇杆Y轴当前值 | Left stick Y axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kRightStickX),  // 右摇杆X轴当前值 | Right stick X axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kRightStickY)   // 右摇杆Y轴当前值 | Right stick Y axis current value
    );
  }
}