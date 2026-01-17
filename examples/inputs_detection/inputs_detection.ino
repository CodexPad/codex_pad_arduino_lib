/**
 * CodexPad 输入状态检测
 * CodexPad State Detection Example
 *
 * 本示例演示如何使用CodexPad手柄进行状态变化检测
 * This example demonstrates how to perform state change detection with CodexPad
 */

#include "codex_pad.h"

namespace {
CodexPad g_codex_pad;

/**
 * 将按钮枚举转换为可读的字符串名称
 * Convert button constant to readable string name
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
      return "SquareX";  // 方形 或者 X 按钮 | SQUARE or X button
    }
    case CodexPad::Button::kTriangleY: {
      return "TriangleY";  // 三角 或者 Y 按钮 | TRIANGLE or Y button
    }
    case CodexPad::Button::kCrossA: {
      return "CrossA";  // 叉型 或者 A 按钮 | CROSS or A button
    }
    case CodexPad::Button::kCircleB: {
      return "CircleB";  // 圆形 或者 B 按钮 | CIRCLE or B button
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
}  // namespace

void setup() {
  Serial.begin(115200);
  printf("Init\n");

  g_codex_pad.Init();

  printf("Begin connecting\n");

  // 连接到指定MAC地址的手柄
  // Connect to the CodexPad with specified MAC address
  const auto ret = g_codex_pad.Connect("E4:66:E5:A2:24:5D");

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
  g_codex_pad.set_tx_power(CodexPad::TxPower::k0dBm);
}

void loop() {
  // 重要：Update()方法必须在循环中尽可能频繁地调用，不能添加延时
  // 该方法负责处理所有接收到的蓝牙数据包，延时会导致数据丢失和响应延迟
  // 对于实时控制应用，必须保持高频率调用以确保及时响应手柄输入
  // Important: Update() method must be called as frequently as possible in the loop, no delays should be added
  // This method processes all received Bluetooth packets, delays will cause data loss and response lag
  // For real-time control applications, high-frequency calls are essential to ensure prompt response to gamepad input
  g_codex_pad.Update();

  // 检测所有按钮的状态变化
  // 使用pressed(), released(), holding()方法检测按钮的不同状态
  // Detect state changes for all buttons
  // Use pressed(), released(), holding() methods to detect different button states
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
    // 检测按钮是否刚刚按下（从弹起变为按下）
    // Check if button was just pressed (transition from released to pressed)
    if (g_codex_pad.pressed(button)) {
      printf("Button %s: pressed\n", ButtonToString(button).c_str());
    }
    // 检测按钮是否刚刚释放（从按下变为弹起）
    // Check if button was just released (transition from pressed to released)
    else if (g_codex_pad.released(button)) {
      printf("Button %s: released\n", ButtonToString(button).c_str());
    }
    // 检测按钮是否持续按下状态
    // Check if button is holding
    else if (g_codex_pad.holding(button)) {
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
  if (g_codex_pad.HasAxisValueChanged(CodexPad::Axis::kLeftStickX, kAxisValueChangeThreshold) ||
      g_codex_pad.HasAxisValueChanged(CodexPad::Axis::kLeftStickY, kAxisValueChangeThreshold) ||
      g_codex_pad.HasAxisValueChanged(CodexPad::Axis::kRightStickX, kAxisValueChangeThreshold) ||
      g_codex_pad.HasAxisValueChanged(CodexPad::Axis::kRightStickY, kAxisValueChangeThreshold)) {
    // 打印摇杆轴的当前值（0-255）
    // Print current joystick axis values (0-255)
    printf("Left stick axis values: [X: %" PRIu8 "], [Y: %" PRIu8 "], Right stick axis values: [X: %" PRIu8 "], [Y: %" PRIu8 "]\n",
           g_codex_pad.axis_value(CodexPad::Axis::kLeftStickX),   // 左摇杆X轴当前值 | Left stick X axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kLeftStickY),   // 左摇杆Y轴当前值 | Left stick Y axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kRightStickX),  // 右摇杆X轴当前值 | Right stick X axis current value
           g_codex_pad.axis_value(CodexPad::Axis::kRightStickY)   // 右摇杆Y轴当前值 | Right stick Y axis current value
    );
  }
}