/**
 * @~English
 * @file basic_polling.ino
 * @example basic_polling.ino
 * @brief CodexPad basic polling example
 */
/**
 * @~Chinese
 * @file basic_polling.ino
 * @example basic_polling.ino
 * @brief CodexPad基本轮询示例
 */

#include "codex_pad.h"

namespace {
CodexPad g_codex_pad;
}  // namespace

void setup() {
  // 初始化串口通信，波特率115200
  // Initialize serial communication, baud rate 115200
  Serial.begin(115200);
  printf("Init\n");

  g_codex_pad.Init("E4:66:E5:A2:24:5D");

  printf("Begin connecting\n");

  // 连接到指定MAC地址的手柄
  // Connect to the CodexPad with specified MAC address
  const auto ret = g_codex_pad.Connect();

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

  if (!g_codex_pad.is_connected()) {
    printf("disconnected, reconnecting...\n");
    const auto ret = g_codex_pad.Connect();
    printf("connected, ret: %d\n", ret);
  }

  static uint32_t s_print_time = 0;
  if (s_print_time == 0 || s_print_time + 30 < millis()) {
    s_print_time = millis();

    printf(
        "Up:%u, Down:%u, Left:%u, Right:%u, Square(X):%u, Triangle(Y):%u, Cross(A):%u, Circle(B):%u, L1:%u, L2:%u, L3:%u, R1:%u, R2:%u, "
        "R3:%u, Select:%u, "
        "Start:%u, Home:%u, L(X:%3u, Y:%3u), R(X:%3u, Y:%3u)\n",

        // 获取各个按钮的状态，button_state()返回bool类型，true表示按下，false表示弹起
        // Get button states, button_state() returns bool type, true means pressed, false means released
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

        // 获取摇杆轴数据，axis_value()返回0~255的数值
        // 中间位置约为128，数值范围表示摇杆的偏移程度
        // Get joystick axis data, axis_value() returns value from 0 to 255
        // Center position is around 128, values represent stick deflection
        g_codex_pad.axis_value(CodexPad::Axis::kLeftStickX),
        g_codex_pad.axis_value(CodexPad::Axis::kLeftStickY),
        g_codex_pad.axis_value(CodexPad::Axis::kRightStickX),
        g_codex_pad.axis_value(CodexPad::Axis::kRightStickY));
  }
}