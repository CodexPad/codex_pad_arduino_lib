#pragma once

#include <BLEDevice.h>

#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <vector>

/**
 * @~English
 * @class CodexPad
 * @brief CodexPad main class
 */
/**
 * @~Chinese
 * @class CodexPad
 * @brief CodexPad主类
 */
class CodexPad {
 public:
  /**
   * @~English
   * @brief Number of axis values
   */
  /**
   * @~Chinese
   * @brief 轴值数量
   */
  static constexpr size_t kAxisValueNum = 4;

  /**
   * @~English
   * @enum CodexPad::TxPower
   * @brief CodexPad transmission power levels
   */
  /**
   * @~Chinese
   * @enum CodexPad::TxPower
   * @brief CodexPad发射功率等级
   */
  enum class TxPower : int8_t {
    /**
     * @~English
     * @brief -16dBm
     */
    /**
     * @~Chinese
     * @brief -16dBm
     */
    kMinus16dBm = -16,

    /**
     * @~English
     * @brief -12dBm
     */
    /**
     * @~Chinese
     * @brief -12dBm
     */
    kMinus12dBm = -12,

    /**
     * @~English
     * @brief -8dBm
     */
    /**
     * @~Chinese
     * @brief -8dBm
     */
    kMinus8dBm = -8,

    /**
     * @~English
     * @brief -5dBm
     */
    /**
     * @~Chinese
     * @brief -5dBm
     */
    kMinus5dBm = -5,

    /**
     * @~English
     * @brief -3dBm
     */
    /**
     * @~Chinese
     * @brief -3dBm
     */
    kMinus3dBm = -3,

    /**
     * @~English
     * @brief -1dBm
     */
    /**
     * @~Chinese
     * @brief -1dBm
     */
    kMinus1dBm = -1,

    /**
     * @~English
     * @brief 0dBm
     */
    /**
     * @~Chinese
     * @brief 0dBm
     */
    k0dBm = 0,

    /**
     * @~English
     * @brief 1dBm
     */
    /**
     * @~Chinese
     * @brief 1dBm
     */
    k1dBm = 1,

    /**
     * @~English
     * @brief 2dBm
     */
    /**
     * @~Chinese
     * @brief 2dBm
     */
    k2dBm = 2,

    /**
     * @~English
     * @brief 3dBm
     */
    /**
     * @~Chinese
     * @brief 3dBm
     */
    k3dBm = 3,

    /**
     * @~English
     * @brief 4dBm
     */
    /**
     * @~Chinese
     * @brief 4dBm
     */
    k4dBm = 4,

    /**
     * @~English
     * @brief 5dBm
     */
    /**
     * @~Chinese
     * @brief 5dBm
     */
    k5dBm = 5,

    /**
     * @~English
     * @brief 6dBm
     */
    /**
     * @~Chinese
     * @brief 6dBm
     */
    k6dBm = 6
  };

  /**
   * @~English
   * @enum CodexPad::Button
   * @brief Button Type of CodexPad
   */
  /**
   * @~Chinese
   * @enum CodexPad::Button
   * @brief CodexPad按键类型
   */
  enum Button : uint32_t {
    /**
     * @~English
     * @brief Up
     */
    /**
     * @~Chinese
     * @brief 上
     */
    kUp = static_cast<uint32_t>(1) << 0,

    /**
     * @~English
     * @brief Down
     */
    /**
     * @~Chinese
     * @brief 下
     */
    kDown = static_cast<uint32_t>(1) << 1,

    /**
     * @~English
     * @brief Left
     */
    /**
     * @~Chinese
     * @brief 左
     */
    kLeft = static_cast<uint32_t>(1) << 2,

    /**
     * @~English
     * @brief Right
     */
    /**
     * @~Chinese
     * @brief 右
     */
    kRight = static_cast<uint32_t>(1) << 3,

    /**
     * @~English
     * @brief Circle or X
     */
    /**
     * @~Chinese
     * @brief 圆形或者X
     */
    kSquareX = static_cast<uint32_t>(1) << 4,

    /**
     * @~English
     * @brief Triangle or Y
     */
    /**
     * @~Chinese
     * @brief 三角形或者Y
     */
    kTriangleY = static_cast<uint32_t>(1) << 5,

    /**
     * @~English
     * @brief Cross or A
     */
    /**
     * @~Chinese
     * @brief 叉或者A
     */
    kCrossA = static_cast<uint32_t>(1) << 6,

    /**
     * @~English
     * @brief Circle or B
     */
    /**
     * @~Chinese
     * @brief 圆形或者B
     */
    kCircleB = static_cast<uint32_t>(1) << 7,

    /**
     * @~English
     * @brief L1
     */
    /**
     * @~Chinese
     * @brief L1
     */
    kL1 = static_cast<uint32_t>(1) << 8,

    /**
     * @~English
     * @brief L2
     */
    /**
     * @~Chinese
     * @brief L2
     */
    kL2 = static_cast<uint32_t>(1) << 9,

    /**
     * @~English
     * @brief L3
     */
    /**
     * @~Chinese
     * @brief L3
     */
    kL3 = static_cast<uint32_t>(1) << 10,

    /**
     * @~English
     * @brief R1
     */
    /**
     * @~Chinese
     * @brief R1
     */
    kR1 = static_cast<uint32_t>(1) << 11,

    /**
     * @~English
     * @brief R2
     */
    /**
     * @~Chinese
     * @brief R2
     */
    kR2 = static_cast<uint32_t>(1) << 12,

    /**
     * @~English
     * @brief R3
     */
    /**
     * @~Chinese
     * @brief R3
     */
    kR3 = static_cast<uint32_t>(1) << 13,

    /**
     * @~English
     * @brief Select
     */
    /**
     * @~Chinese
     * @brief 选择
     */
    kSelect = static_cast<uint32_t>(1) << 14,

    /**
     * @~English
     * @brief Start
     */
    /**
     * @~Chinese
     * @brief 开始
     */
    kStart = static_cast<uint32_t>(1) << 15,

    /**
     * @~English
     * @brief Home
     */
    /**
     * @~Chinese
     * @brief 首页
     */
    kHome = static_cast<uint32_t>(1) << 16,
  };

  /**
   * @~English
   * @enum CodexPad::Axis
   * @brief Axis
   */
  /**
   * @~Chinese
   * @enum CodexPad::Axis
   * @brief 轴
   */
  enum Axis : size_t {
    /**
     * @~English
     * @brief Left stick X axis
     */
    /**
     * @~Chinese
     * @brief 左摇杆X轴
     */
    kLeftStickX,

    /**
     * @~English
     * @brief Left stick Y axis
     */
    /**
     * @~Chinese
     * @brief 左摇杆Y轴
     */
    kLeftStickY,

    /**
     * @~English
     * @brief Right stick X axis
     */
    /**
     * @~Chinese
     * @brief 右摇杆X轴
     */
    kRightStickX,

    /**
     * @~English
     * @brief Right stick Y axis
     */
    /**
     * @~Chinese
     * @brief 右摇杆Y轴
     */
    kRightStickY,
  };

  /**
   * @~English
   * @brief Constructor
   */
  /**
   * @~Chinese
   * @brief 构造函数
   */
  CodexPad();

  /**
   * @~English
   * @brief Destructor
   */
  /**
   * @~Chinese
   * @brief 析构函数
   */
  ~CodexPad();

  /**
   * @~English
   * @brief Initialize
   */
  /**
   * @~Chinese
   * @brief 初始化
   */
  void Init();

  /**
   * @~English
   * @brief Connect
   * @param[in] mac_address MAC address of the CodexPad, formatted as "XX:XX:XX:XX:XX:XX", X is a number or uppercase letter, colon separated
   * @param[in] timeout_ms Timeout in milliseconds
   * @retval true if connected successfully
   * @retval false if connection failed
   * */
  /**
   * @~Chinese
   * @brief 连接
   * @param[in] timeout_ms 超时时间，单位毫秒
   * @param[in] mac_address CodexPad的MAC地址，格式为"XX:XX:XX:XX:XX:XX"，X为数字或者大写字母, 半角冒号分隔
   * @retval true 连接成功
   * @retval false 连接失败
   */
  bool Connect(const std::string& mac_address, const uint32_t timeout_ms = UINT32_MAX);
  /**
   * @~English
   * @brief Update, need to be called in Loop
   */
  /**
   * @~Chinese
   * @brief 更新，需要在Loop中不断调用
   */
  void Update();

  /**
   * @~English
   * @brief Is connected
   * @retval true if connected
   * @retval false if not
   */
  /**
   * @~Chinese
   * @brief 是否连接
   * @retval true 已连接
   * @retval false 未连接
   */
  inline bool is_connected() const {
    return connection_state_;
  }

  /**
   * @~English
   * @brief Set transmission power, only effective when connected, immediately effective for current connection, effective for next connection
   * @param[in] power Transmission power
   */
  /**
   * @~Chinese
   * @brief 设置发射功率，连接状态下调用，立即生效于当前连接，下次连接生效
   * @param[in] power 发射功率
   */
  void set_tx_power(const TxPower power);

  /**
   * @~English
   * @brief Get model number of the CodexPad
   * @return Model number of the CodexPad
   */
  /**
   * @~Chinese
   * @brief 获取CodexPad的型号
   * @return CodexPad的型号
   */
  inline const std::string& device_name() const {
    return device_name_;
  }

  /**
   * @~English
   * @brief Get model number of the CodexPad
   * @return Model number of the CodexPad
   */
  /**
   * @~Chinese
   * @brief 获取CodexPad的型号
   * @return CodexPad的型号
   */
  inline const std::string& model_number() const {
    return model_number_;
  }

  /**
   * @~English
   * @brief check if a button is pressed
   * @retval true if the button is pressed
   * @retval false if the button is not pressed
   */
  /**
   * @~Chinese
   * @brief 查询按键是否被按下
   * @retval true 按键被按下
   * @retval false 按键没有被按下
   */
  bool pressed(const Button button) const;

  /**
   * @~English
   * @brief check if a button is released
   * @retval true if the button is released
   * @retval false if the button is not released
   */
  /**
   * @~Chinese
   * @brief 查询按键是否被释放
   * @retval true 按键被释放
   * @retval false 按键没有被释放
   */
  bool released(const Button button) const;

  /**
   * @~English
   * @brief check if a button is held
   * @retval true if the button is held
   * @retval false if the button is not held
   */
  /**
   * @~Chinese
   * @brief 查询按键是否被持续按下
   * @retval true 按键被持续按下
   * @retval false 按键没有被持续按下
   */
  bool holding(const Button button) const;

  /**
   * @~English
   * @brief check if a button is pressed or held
   * @retval true if the button is pressed or held
   * @retval false if the button is not pressed or held
   */
  /**
   * @~Chinese
   * @brief 查询按键是否被按下或持续按下
   * @retval true 按键被按下或持续按下
   * @retval false 按键没有被按下或持续按下
   */
  bool button_state(const Button button) const;

  /**
   * @~English
   * @brief Get all button states, return a 32-bit unsigned integer where each bit represents the state of a specific button
   * @return A 32-bit unsigned integer where each bit represents the state of a specific button
   *         The bits are from the least significant bit (LSB, bit 0) to the most significant bit (MSB, bit 31).
   *         You can use bit-and (&) operator to check specific button states.
   * Example:
   * @code
   *   const uint32_t button_states = codex_pad.button_states();
   *   if ((button_states & CodexPad::Button::kUp) != 0) {
   *     // Up button is pressed
   *   }
   * @endcode
   */
  /**
   * @~Chinese
   * @brief 以位掩码形式获取所有按键的当前状态
   * @return 一个32位无符号整数，每一位（bit）表示 @ref Button 枚举中对应按键的状态（1为按下，0为未按下）。
   *         位的分配从最低有效位（LSB，第0位）开始，依次对应 @ref Button 中的各个值。
   *         可使用位与操作 (&) 配合具体的 @ref Button 枚举值来检查特定按键状态。
   * 示例：
   * @code
   *   const uint32_t button_states = codex_pad.button_states();
   *   if ((button_states & CodexPad::Button::kUp)) != 0) {
   *     // 方向上按键被按下
   *   }
   * @endcode
   */
  uint32_t button_states() const;

  /**
   * @~English
   * @brief Get axis value
   * @param[in] axis Axis
   * @return Axis value, range 0~255
   */
  /**
   * @~Chinese
   * @brief 获取轴值
   * @param[in] axis 轴
   * @return 轴值，范围 0~255
   */
  uint8_t axis_value(const Axis axis) const;

  /**
   * @~English
   * @brief Get current values of all analog axes
   * @return Array of axis values with length @ref kAxisValueNum (4 elements), where each element represents the position of a specific axis in the
   * range 0-255. The array indices correspond to the @ref Axis enumeration:
   * - Index 0: Left stick X axis (kLeftStickX)
   * - Index 1: Left stick Y axis (kLeftStickY)
   * - Index 2: Right stick X axis (kRightStickX)
   * - Index 3: Right stick Y axis (kRightStickY)
   *
   * Typical center position value is 128 (0x80).
   *
   * Example:
   * @code
   *   const auto axes = codex_pad.axis_values();
   *   const uint8_t left_stick_x = axes[CodexPad::Axis::kLeftStickX]; // Get the value of the left stick X axis
   * @endcode
   */
  /**
   * @~Chinese
   * @brief 获取所有模拟轴的当前值
   * @return 长度为 @ref  kAxisValueNum (4个元素) 的轴值数组，每个元素代表特定轴的位置，范围 0-255。数组索引与 @ref Axis 枚举对应关系：
   * - 索引 0：左摇杆X轴 (@ref Axis::kLeftStickX)
   * - 索引 1：左摇杆Y轴 (@ref Axis::kLeftStickY)
   * - 索引 2：右摇杆X轴 (@ref Axis::kRightStickX)
   * - 索引 3：右摇杆Y轴 (@ref Axis::kRightStickY)
   *
   * 典型中心位置值为 128 (0x80)。
   *
   * 示例：
   * @code
   *   const auto axes = codex_pad.axis_values();
   *   const uint8_t left_stick_x = axes[CodexPad::Axis::kLeftStickX]; // 获取左摇杆X轴值
   * @endcode
   */
  std::array<uint8_t, kAxisValueNum> axis_values() const;

  /**
   * @~English
   * @brief check if an axis value has changed
   * @param[in] axis Axis
   * @param[in] threshold Threshold
   * @retval true if the axis value has changed
   * @retval false if the axis value has not changed
   */
  /**
   * @~Chinese
   * @brief 查询轴值是否发生变化
   * @param[in] axis 轴
   * @param[in] threshold 阈值
   * @retval true 轴值发生变化
   * @retval false 轴值未发生变化
   */
  bool HasAxisValueChanged(const Axis axis, const uint8_t threshold) const;

 private:
  static constexpr size_t kButtonNum = 32;
  static constexpr size_t kInputsQueueMax = 10;

  struct Inputs {
    uint32_t button_states = 0;
    uint8_t axis_values[kAxisValueNum] = {0x80, 0x80, 0x80, 0x80};
  };

  void OnNotify(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t length, bool is_notify);

  mutable std::mutex mutex_;
  BLEAddress address_;
  BLEClient* ble_client_ = nullptr;
  std::string device_name_;
  std::string model_number_;
  uint8_t device_firmware_version_[3] = {0};
  Inputs prev_inputs_;
  Inputs current_inputs_;
  std::queue<Inputs> inputs_queue_;
  bool connection_state_ = false;
};