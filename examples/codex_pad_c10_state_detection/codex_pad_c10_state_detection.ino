/**
 * CodexPadC10 状态检测示例
 * CodexPadC10 State Detection Example
 *
 * 本示例演示如何使用CodexPadC10 BLE手柄进行状态变化检测
 * This example demonstrates how to perform state change detection with CodexPadC10 BLE gamepad
 */

#include "codex_pad_c10.h"

namespace {
// 全局CodexPadC10实例
// Global CodexPadC10 instance
CodexPadC10 g_codex_pad_c10;

/**
 * 将按钮枚举转换为可读的字符串名称
 * Convert button constant to readable string name
 */
std::string ButtonToString(CodexPadC10::Button button) {
  switch (button) {
    case CodexPadC10::Button::kL3: {
      return "L3";  // 左摇杆按下触发 | LEFT_STICK button press
    }
    case CodexPadC10::Button::kStart: {
      return "START";  // START按钮 | START button
    }
    case CodexPadC10::Button::kX: {
      return "X";  // X按钮 | X button
    }
    case CodexPadC10::Button::kY: {
      return "Y";  // Y按钮 | Y button
    }
    case CodexPadC10::Button::kA: {
      return "A";  // A按钮 | A button
    }
    case CodexPadC10::Button::kB: {
      return "B";  // B按钮 | B button
    }
    default: {
      return {};  // 未知按钮返回空字符串 | Unknown button returns empty string
    }
  }
}
}  // namespace

void setup() {
  // 初始化串口通信，波特率115200
  // Initialize serial communication, baud rate 115200
  Serial.begin(115200);
  printf("Init\n");

  // 初始化CodexPadC10手柄
  // Initialize CodexPadC10 gamepad
  g_codex_pad_c10.Init();

  printf("Begin connecting\n");

  // 连接到指定MAC地址的手柄
  // Connect to the CodexPad with specified MAC address
  const auto ret = g_codex_pad_c10.Connect("E4:66:E5:A2:24:5D");

  // 检查连接结果
  // Check connection result
  if (ret) {
    printf("Connected\n");
  } else {
    printf("Connect failed\n");
    // 连接失败，进入无限循环
    // Connection failed, enter infinite loop
    while (true);
  }

  // 设置发射功率为0dBm
  // 发射功率影响通信距离和功耗：功率越高，通信距离越远，但功耗也越大
  // 建议根据实际应用场景选择合适的功率等级以平衡距离和电池寿命
  // Set transmission power to 0dBm
  // Transmission power affects communication range and power consumption:
  // Higher power provides longer range but consumes more battery
  // Choose appropriate power level based on your application to balance range and battery life
  g_codex_pad_c10.set_tx_power(CodexPadC10::TxPower::k0dBm);
}

void loop() {
  // 重要：Update()方法必须在循环中尽可能频繁地调用，不能添加延时
  // 该方法负责处理所有接收到的蓝牙数据包，延时会导致数据丢失和响应延迟
  // 对于实时控制应用，必须保持高频率调用以确保及时响应手柄输入
  // Important: Update() method must be called as frequently as possible in the loop, no delays should be added
  // This method processes all received Bluetooth packets, delays will cause data loss and response lag
  // For real-time control applications, high-frequency calls are essential to ensure prompt response to gamepad input
  g_codex_pad_c10.Update();

  // 检测所有按钮的状态变化
  // 使用pressed(), released(), holding()方法检测按钮的不同状态
  // Detect state changes for all buttons
  // Use pressed(), released(), holding() methods to detect different button states
  for (auto button : {CodexPadC10::Button::kL3,
                      CodexPadC10::Button::kStart,
                      CodexPadC10::Button::kX,
                      CodexPadC10::Button::kY,
                      CodexPadC10::Button::kA,
                      CodexPadC10::Button::kB}) {
    // 检测按钮是否刚刚按下（从弹起变为按下）
    // Check if button was just pressed (transition from released to pressed)
    if (g_codex_pad_c10.pressed(button)) {
      printf("Button %s: pressed\n", ButtonToString(button).c_str());
    }
    // 检测按钮是否刚刚释放（从按下变为弹起）
    // Check if button was just released (transition from pressed to released)
    else if (g_codex_pad_c10.released(button)) {
      printf("Button %s: released\n", ButtonToString(button).c_str());
    }
    // 检测按钮是否持续按下状态
    // Check if button is holding
    else if (g_codex_pad_c10.holding(button)) {
      printf("Button %s: holding\n", ButtonToString(button).c_str());
    }
  }

  // 检测摇杆轴值是否发生了有效变化（使用阈值避免微小抖动）
  // 阈值设置为2，只有当摇杆值变化达到或超过2个单位时才认为是有效变化
  // Check if joystick axis values have changed significantly (using threshold to avoid minor jitter)
  // Threshold is set to 2, only consider changes equal to or greater than 2 units as significant
  constexpr uint8_t kAxisValueChangeThreshold = 2;

  // 检测左摇杆X轴或Y轴是否有显著变化
  // Check if left stick X or Y axis has significant change
  if (g_codex_pad_c10.HasAxisValueChanged(CodexPadC10::Axis::kLeftStickX, kAxisValueChangeThreshold) ||
      g_codex_pad_c10.HasAxisValueChanged(CodexPadC10::Axis::kLeftStickY, kAxisValueChangeThreshold)) {
    // 打印摇杆轴的当前值（0-255）
    // Print current joystick axis values (0-255)
    printf("Left stick axis values: [X: %" PRIu8 "], [Y: %" PRIu8 "]\n",
           g_codex_pad_c10.axis_value(CodexPadC10::Axis::kLeftStickX),  // 左摇杆X轴当前值 | Left stick X axis current value
           g_codex_pad_c10.axis_value(CodexPadC10::Axis::kLeftStickY)   // 左摇杆Y轴当前值 | Left stick Y axis current value
    );
  }
}