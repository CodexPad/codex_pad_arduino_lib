/**
 * CodexPadC10 基础轮询示例
 * CodexPadC10 Basic Polling Example
 *
 * 本示例演示如何使用CodexPadC10 BLE手柄进行基础状态轮询
 * This example demonstrates how to perform basic state polling with CodexPadC10 BLE gamepad
 */

#include "codex_pad_c10.h"

namespace {
// 全局CodexPadC10实例
// Global CodexPadC10 instance
CodexPadC10 g_codex_pad_c10;
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

  // 获取各个按钮的状态，button_state()返回bool类型，true表示按下，false表示弹起
  // 注意：kL3对应左摇杆按下操作，不是摇杆移动
  // Get button states, button_state() returns bool type, true means pressed, false means released
  // Note: kL3 corresponds to left stick button press, not stick movement
  const bool button_l3_state = g_codex_pad_c10.button_state(CodexPadC10::Button::kL3);        // 左摇杆按下状态 | Left stick button state
  const bool button_start_state = g_codex_pad_c10.button_state(CodexPadC10::Button::kStart);  // START按钮状态 | START button state
  const bool button_x_state = g_codex_pad_c10.button_state(CodexPadC10::Button::kX);          // X按钮状态 | X button state
  const bool button_y_state = g_codex_pad_c10.button_state(CodexPadC10::Button::kY);          // Y按钮状态 | Y button state
  const bool button_a_state = g_codex_pad_c10.button_state(CodexPadC10::Button::kA);          // A按钮状态 | A button state
  const bool button_b_state = g_codex_pad_c10.button_state(CodexPadC10::Button::kB);          // B按钮状态 | B button state

  // 获取摇杆轴数据，axis_value()返回0~255的数值
  // 中间位置约为127，数值范围表示摇杆的偏移程度
  // Get joystick axis data, axis_value() returns value from 0 to 255
  // Center position is around 127, values represent stick deflection
  const uint8_t left_stick_x_axis_value = g_codex_pad_c10.axis_value(CodexPadC10::Axis::kLeftStickX);  // 左摇杆X轴数值 | Left stick X axis value
  const uint8_t left_stick_y_axis_value = g_codex_pad_c10.axis_value(CodexPadC10::Axis::kLeftStickY);  // 左摇杆Y轴数值 | Left stick Y axis value

  // 打印当前状态 - 便于调试和监控手柄输入
  // Print current status - useful for debugging and monitoring gamepad input
  printf("Button States: [L3: %u], [START: %u], [X: %u], [Y: %u], [A: %u], [B: %u], Left stick axis values: [X: %u], [Y: %u]\n",
         button_l3_state,
         button_start_state,
         button_x_state,
         button_y_state,
         button_a_state,
         button_b_state,
         left_stick_x_axis_value,
         left_stick_y_axis_value);
}