#pragma once

#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include "NimBLEDevice.h"
#include "gamepad_input_tracker.h"

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
     * @brief -16 dBm
     */
    /**
     * @~Chinese
     * @brief -16 dBm
     */
    kMinus16dBm = -16,

    /**
     * @~English
     * @brief -12 dBm
     */
    /**
     * @~Chinese
     * @brief -12 dBm
     */
    kMinus12dBm = -12,

    /**
     * @~English
     * @brief -8 dBm
     */
    /**
     * @~Chinese
     * @brief -8 dBm
     */
    kMinus8dBm = -8,

    /**
     * @~English
     * @brief -5 dBm
     */
    /**
     * @~Chinese
     * @brief -5 dBm
     */
    kMinus5dBm = -5,

    /**
     * @~English
     * @brief -3 dBm
     */
    /**
     * @~Chinese
     * @brief -3 dBm
     */
    kMinus3dBm = -3,

    /**
     * @~English
     * @brief -1 dBm
     */
    /**
     * @~Chinese
     * @brief -1 dBm
     */
    kMinus1dBm = -1,

    /**
     * @~English
     * @brief 0 dBm
     */
    /**
     * @~Chinese
     * @brief 0 dBm
     */
    k0dBm = 0,

    /**
     * @~English
     * @brief 1 dBm
     */
    /**
     * @~Chinese
     * @brief 1 dBm
     */
    k1dBm = 1,

    /**
     * @~English
     * @brief 2 dBm
     */
    /**
     * @~Chinese
     * @brief 2 dBm
     */
    k2dBm = 2,

    /**
     * @~English
     * @brief 3 dBm
     */
    /**
     * @~Chinese
     * @brief 3 dBm
     */
    k3dBm = 3,

    /**
     * @~English
     * @brief 4 dBm
     */
    /**
     * @~Chinese
     * @brief 4 dBm
     */
    k4dBm = 4,

    /**
     * @~English
     * @brief 5 dBm
     */
    /**
     * @~Chinese
     * @brief 5 dBm
     */
    k5dBm = 5,

    /**
     * @~English
     * @brief 6 dBm
     */
    /**
     * @~Chinese
     * @brief 6 dBm
     */
    k6dBm = 6
  };
  /**
   * @~English
   * @brief Constructor
   */
  /**
   * @~Chinese
   * @brief 构造函数
   */
  CodexPad() noexcept;

  /**
   * @~English
   * @brief Destructor
   */
  /**
   * @~Chinese
   * @brief 析构函数
   */
  ~CodexPad() noexcept;

  /**
   * @~English
   * @brief Initialize
   */
  /**
   * @~Chinese
   * @brief 初始化
   */
  void Init() noexcept;

  /**
   * @~English
   * @brief Connect
   * @param[in] bluetooth_device_address The Bluetooth device address(BD_ADDR) of the CodexPad, formatted as
   * "XX:XX:XX:XX:XX:XX", X is a number or uppercase letter, colon separated
   * @param[in] timeout_ms Timeout in milliseconds
   * @retval true if connected successfully
   * @retval false if connection failed
   * */
  /**
   * @~Chinese
   * @brief 连接
   * @param[in] bluetooth_device_address CodexPad的蓝牙设备地址（BD_ADDR），格式为"XX:XX:XX:XX:XX:XX"，X为数字或者大写字母,
   * 半角冒号分隔
   * @param[in] timeout_ms 超时时间，单位毫秒
   * @retval true 连接成功
   * @retval false 连接失败
   */
  bool Connect(const std::string& bluetooth_device_address, uint32_t timeout_ms = 5000) noexcept;

  /**
   * @~English
   * @brief Scans for nearby CodexPad devices and automatically connects to a device whose button state matches the specified
   * mask.
   * @details This method actively scans for Bluetooth devices. When it discovers one or more CodexPad devices whose current
   * button states exactly match the provided `button_mask`, it will attempt to establish a connection. If multiple nearby
   * devices match the button mask, the device with the strongest signal (highest RSSI) will be selected for connection.
   * @param[in] button_mask A 32-bit button mask used to match the target device's button states.
   *                        Use the `ButtonMask()` function to combine multiple @ref Button values.
   * @retval true Connection successful (a device matching the button mask was found and connected).
   * @retval false Connection failed (no matching device found or connection attempt failed).
   * @note This is a blocking call. It will continuously scan until a matching device is found, a connection is established, or
   * the internal timeout is reached.
   * @warning **DO NOT** include `Button::kHome` in the button mask. Holding the Home button triggers a device reboot, which
   * will interrupt the connection process. Example:
   * @code
   *   CodexPad pad;
   *   pad.Init();
   *   // Continuously try to connect to a device where the Start and Cross/A buttons are pressed.
   *   while (!pad.ScanAndConnect(gamepad::input::Button::kStart | gamepad::input::Button::kStart)) {
   *     // Optional: Add a small delay or other operations between attempts.
   *     delay(100);
   *   }
   *   // Once the loop exits, connection is successful.
   *   Serial.println("CodexPad connected!");
   * @endcode
   */
  /**
   * @~Chinese
   * @brief 扫描附近的 CodexPad 设备，并自动连接到一个按键状态与指定掩码匹配的设备。
   * @details 此方法会主动扫描蓝牙设备。当发现一个或多个 CodexPad 设备，其当前按键状态与提供的 `button_mask`
   * 完全匹配时，它将尝试建立连接。 如果附近有多个设备都符合按键掩码，则会选择信号最强（RSSI 值最大）的设备进行连接。
   * @param[in] button_mask 用于匹配目标设备按键状态的32位按钮掩码。
   *                        使用 `ButtonMask()` 函数来组合多个 @ref Button 枚举值。
   * @retval true 连接成功（找到了符合按键掩码的设备并成功连接）。
   * @retval false 连接失败（未找到匹配设备或连接尝试失败）。
   * @note 这是一个阻塞式调用。它会持续扫描，直到找到匹配的设备、连接建立或达到内部超时时间。
   * @warning **请勿**在按钮掩码中包含 `Button::kHome`。按住 Home 键会触发设备重启，这将中断连接过程。
   * 示例：
   * @code
   *   CodexPad pad;
   *   pad.Init();
   *   // 持续尝试连接到 Start 和 Cross/A 按键被同时按下的设备。
   *   while (!pad.ScanAndConnect(gamepad::input::Button::kStart | gamepad::input::Button::kStart)) {
   *     // 可选：在尝试之间添加短暂延时或其他操作。
   *     delay(100);
   *   }
   *   // 循环退出，意味着连接成功。
   *   Serial.println("CodexPad 连接成功！");
   * @endcode
   */
  bool ScanAndConnect(gamepad::input::Button buttons) noexcept;

  /**
   * @~English
   * @brief Disconnect
   */
  /**
   * @~Chinese
   * @brief 断开连接
   */
  void Disconnect() noexcept { Reset(); }

  /**
   * @~English
   * @brief Update, need to be called in Loop
   */
  /**
   * @~Chinese
   * @brief 更新，需要在Loop中不断调用
   */
  const gamepad::input::Tracker& Update() noexcept;

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
  bool is_connected() const noexcept;

  /**
   * @~English
   * @brief Set transmission power, only effective when connected, immediately effective for current connection, effective for
   * next connection
   * @param[in] power Transmission power
   * @retval true Success
   * @retval false Fail
   */
  /**
   * @~Chinese
   * @brief 设置发射功率，连接状态下调用，立即生效于当前连接，下次连接生效
   * @param[in] power 发射功率
   * @retval true 成功
   * @retval false 失败
   */
  bool set_remote_tx_power(TxPower power) noexcept;

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
  const std::string& remote_device_name() const noexcept { return remote_device_name_; }

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
  const std::string& remote_model_number() const noexcept { return remote_model_number_; }

  /**
   * @~English
   * @brief Get firmware version of the CodexPad
   * @return Firmware version of the CodexPad
   */
  /**
   * @~Chinese
   * @brief 获取CodexPad的固件版本
   * @return CodexPad的固件版本
   */
  const std::array<uint8_t, 3>& remote_firmware_version() const noexcept { return remote_firmware_version_; }

  /**
   * @~English
   * @brief Get the input tracker object
   * @details This function returns a reference to the internal input tracker object.
   * @return Input tracker object
   */
  /**
   * @~Chinese
   * @brief 获取输入跟踪器对象
   * @details 此函数返回内部输入跟踪器对象的引用。
   * @return 输入跟踪器对象
   */
  const gamepad::input::Tracker& input_tracker() const noexcept { return input_tracker_; }

  /**
   * @~English
   * @brief Get the BLE client object
   * @details This function returns a pointer to the internal BLE client object. The caller should not try and release/delete
   * it.
   * @return BLE client object
   */
  /**
   * @~Chinese
   * @brief 获取 BLE 客户端对象
   * @details 此函数返回内部 BLE 客户端对象的指针。调用者不应尝试释放/删除它。
   * @return BLE 客户端对象
   */
  NimBLEClient* ble_client() const noexcept { return ble_client_; }

 private:
  static constexpr size_t kInputsQueueMax = 10;

  bool Connect(const NimBLEAddress& address, bool async_connect, const uint32_t timeout_ms);
  void OnNotify(const NimBLERemoteCharacteristic* remote_characteristic, const uint8_t* data, const size_t length,
                const bool is_notify);
  void Reset();

  mutable std::mutex mutex_;
  NimBLEClient* ble_client_ = nullptr;
  std::string remote_device_name_;
  std::string remote_model_number_;
  std::array<uint8_t, 3> remote_firmware_version_ = {0, 0, 0};
  std::queue<gamepad::input::State> inputs_queue_;
  gamepad::input::Tracker input_tracker_;
};