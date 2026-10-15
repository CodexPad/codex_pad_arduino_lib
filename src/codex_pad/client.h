#pragma once

#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include "NimBLEDevice.h"
#include "cyf.h"
#include "cyf/hysteresis_quantizer.h"
#include "gamepad_input_tracker.h"

namespace codex_pad {

/**
 * @~English
 * @class Client
 * @brief Client main class
 */
/**
 * @~Chinese
 * @class Client
 * @brief Client主类
 */
class Client : public NimBLEClientCallbacks {
 public:
  /**
   * @~English
   * @brief Constructor
   */
  /**
   * @~Chinese
   * @brief 构造函数
   */
  Client() noexcept;

  /**
   * @~English
   * @brief Destructor
   */
  /**
   * @~Chinese
   * @brief 析构函数
   */
  ~Client() noexcept;

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
   * @param[in] address The Bluetooth device address(BD_ADDR) of the CodexPad, formatted as
   * "XX:XX:XX:XX:XX:XX", X is a number or uppercase letter, colon separated
   * @param[in] timeout_ms Timeout in milliseconds
   * @retval true if connected successfully
   * @retval false if connection failed
   * */
  /**
   * @~Chinese
   * @brief 连接
   * @param[in] address CodexPad的蓝牙设备地址，格式为"XX:XX:XX:XX:XX:XX"，X为数字或者大写字母,
   * 半角冒号分隔
   * @param[in] timeout_ms 超时时间，单位毫秒
   * @retval true 连接成功
   * @retval false 连接失败
   */
  bool Connect(const std::string& address, uint32_t timeout_ms = 5000) noexcept;

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
   *   codex_pad::Client g_codex_pad_client;
   *   g_codex_pad_client.Init();
   *   // Continuously try to connect to a device where the Start and Cross/A buttons are pressed.
   *   while (!g_codex_pad_client.ScanAndConnect(gamepad::input::Button::kStart | gamepad::input::Button::kStart)) {
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
   *   codex_pad::Client g_codex_pad_client;
   *   g_codex_pad_client.Init();
   *   // 持续尝试连接到 Start 和 Cross/A 按键被同时按下的设备。
   *   while (!g_codex_pad_client.ScanAndConnect(gamepad::input::Button::kStart | gamepad::input::Button::kStart)) {
   *     // 可选：在尝试之间添加短暂延时或其他操作。
   *     delay(100);
   *   }
   *   // 循环退出，意味着连接成功。
   *   Serial.println("CodexPad 连接成功！");
   * @endcode
   */
  bool Connect(gamepad::input::Button buttons) noexcept;

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
   * @brief Get the RSSI of the connected CodexPad.
   * @details This function retrieves the current Received Signal Strength Indication (RSSI) from the
   *          underlying NimBLE client.
   *          **Note:** On ESP32 Arduino Core versions older than 3.1.0, RSSI readings suffer from severe
   *          fluctuation (drift) and are highly unreliable. Relying on this return value for logic or
   *          display purposes in those older versions is **not recommended**. For accurate RSSI measurements,
   *          please upgrade to ESP32 Arduino Core 3.1.0 or later.
   * @return The current RSSI value in dBm. Returns 0 if the BLE client is not initialized or not connected.
   */
  /**
   * @~Chinese
   * @brief 获取已连接 CodexPad 的 RSSI（接收信号强度指示）。
   * @details 该函数从底层 NimBLE 客户端获取当前的接收信号强度指示（RSSI）值。
   *          **注意：** 在低于 3.1.0 的 ESP32 Arduino Core 版本中，RSSI 读数存在严重漂移（不准确）。
   *          不推荐在旧版本中依赖此返回值进行逻辑判断或界面显示。如需准确的 RSSI 测量，
   *          请将 ESP32 Arduino Core 升级至 3.1.0 或更高版本。
   * @return 当前 RSSI 值，单位为 dBm。如果 BLE 客户端未初始化或未连接，则返回 0。
   */
  int32_t rssi() const noexcept;

  /**
   * @~English
   * @brief Set the local BLE TX power (manual mode).
   * @details This function stores the desired TX power level. However, **if dynamic TX power adjustment is enabled**
   *          (which is the default, see @ref set_dynamic_tx_power_enabled), this value will be ignored and overwritten
   *          by the dynamic adjustment logic during @ref Update().
   *          To ensure this manual setting takes effect persistently, you must first call
   *          `set_dynamic_tx_power_enabled(false)`. On ESP32 Arduino Core versions older than 3.1.0, where dynamic
   *          adjustment is internally disabled due to RSSI drift, this setting will apply normally.
   * @param tx_power Local BLE TX power in dBm.
   */
  /**
   * @~Chinese
   * @brief 设置本地 BLE 发射功率（手动模式）。
   * @details 该函数用于保存期望的发射功率值。但是，**如果动态发射功率调节处于开启状态**（默认开启，
   *          参见 @ref set_dynamic_tx_power_enabled），该值将被忽略，并在 @ref Update() 中被动态调节逻辑覆盖。
   *          如需使该手动设置持续生效，必须先调用 `set_dynamic_tx_power_enabled(false)` 关闭动态调节功能。
   *          在低于 3.1.0 的 ESP32 Arduino Core 版本中，由于内部因 RSSI 漂移已禁用动态调节，此设置可正常生效。
   * @param tx_power 本地 BLE 发射功率，单位 dBm。
   */
  void set_local_tx_power(int32_t tx_power) noexcept { local_tx_power_ = tx_power; }

  /**
   * @~English
   * @brief Get the manually configured local BLE TX power value.
   * @details This function returns the value previously stored by @ref set_local_tx_power() (or its default).
   *          **Note:** When dynamic TX power adjustment is enabled (which is the default, see
   *          @ref set_dynamic_tx_power_enabled), the actual TX power is controlled automatically by the
   *          RSSI-based hysteresis quantizer. In that case, the value returned by this function **does not
   *          reflect the real-time TX power** being used; it only reflects the stored manual setting.
   * @return The stored local BLE TX power setting in dBm.
   */
  /**
   * @~Chinese
   * @brief 获取用户设定的本地 BLE 发射功率值。
   * @details 该函数返回之前通过 @ref set_local_tx_power() 保存的值（或其默认值）。
   *          **注意：** 当动态发射功率调节处于开启状态（默认开启，参见 @ref set_dynamic_tx_power_enabled）
   *          时，实际发射功率由基于 RSSI 的滞环量化器自动控制。此时，该函数返回的值**不能代表当前实时
   *          使用的发射功率**，它仅代表内部存储的手动设定值。
   * @return 存储的本地 BLE 发射功率设定值，单位 dBm。
   */
  int32_t local_tx_power() const noexcept { return local_tx_power_; }

  /**
   * @~English
   * @brief Enable or disable dynamic TX power adjustment based on RSSI.
   * @details This feature is **enabled by default** and **highly recommended** to keep enabled.
   *          When enabled, the client periodically reads the peer's RSSI during @ref Update() and adjusts
   *          the local BLE transmit power using a hysteresis quantizer:
   *          - Strong signal (high RSSI) → Lower TX power to save energy and reduce interference.
   *          - Weak signal (low RSSI)   → Higher TX power to maintain connection stability.
   *          **Conflict with manual power:** When dynamic adjustment is enabled, any value set via
   *          @ref set_local_tx_power() will be **overwritten** and not take effect. To use a fixed manual
   *          TX power, you must first disable this feature by calling `set_dynamic_tx_power_enabled(false)`.
   * @note On ESP32, this feature relies on @ref NimBLEClient::getRssi(). Due to severe RSSI fluctuation (drift)
   *       in ESP32 Arduino Core versions **older than 3.1.0**, the actual adjustment is only compiled and
   *       executed when `ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 1, 0)`. In earlier versions,
   *       the logic is disabled via preprocessor macros, allowing @ref set_local_tx_power() to work as a fallback.
   * @param enabled true to enable (recommended), false to disable and switch to manual control.
   */
  /**
   * @~Chinese
   * @brief 使能或禁用以 RSSI 为基础的动态发射功率调节。
   * @details 该功能**默认开启**，且**强烈建议保持开启**。启用后，客户端会在 @ref Update() 中周期性读取对端设备的
   *          RSSI，并通过滞环量化器（Hysteresis Quantizer）动态调整本地 BLE 发射功率：
   *          - 信号较好（RSSI 较高）时，降低发射功率以节省能耗并减少无线干扰；
   *          - 信号较差（RSSI 较低）时，提高发射功率以维持连接稳定性。
   *          **与手动功率的冲突：** 当动态调节开启时，通过 @ref set_local_tx_power() 设置的任何功率值都会被
   *          **覆盖**，无法生效。如果需要使用固定的手动发射功率，必须先调用
   *          `set_dynamic_tx_power_enabled(false)` 关闭此功能。
   * @note 在 ESP32 平台上，该功能依赖 @ref NimBLEClient::getRssi()。由于在 **3.1.0 之前**的 ESP32 Arduino Core
   *       版本中 RSSI 读数存在严重漂移（不准确），实际调节逻辑仅在 `ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 1, 0)`
   *       时才会编译和执行。在更早的版本中，内部已通过预处理器宏禁用动态调节，此时 @ref set_local_tx_power()
   *       可以正常生效作为回退方案。
   * @param enabled true 表示开启（推荐），false 表示关闭并切换为手动控制模式。
   */
  void set_dynamic_tx_power_enabled(bool enabled) noexcept { dynamic_tx_power_enabled_ = enabled; }
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
  bool onConnParamsUpdateRequest(NimBLEClient*, const ble_gap_upd_params*) override {
    return false;  // update our own connection parameters, so we reject the request
  }

  void onPhyUpdate(NimBLEClient*, uint8_t tx_phy, uint8_t rx_phy) override;
  bool Connect(const NimBLEAddress& address, bool async_connect, const uint32_t timeout_ms);
  void OnNotify(const NimBLERemoteCharacteristic* remote_characteristic, const uint8_t* data, const size_t length,
                const bool is_notify);
  void Reset();
  void UpdateTxPower();

  mutable std::mutex mutex_;
  NimBLEClient* ble_client_{nullptr};
  std::string remote_device_name_;
  std::string remote_model_number_;
  std::array<uint8_t, 3> remote_firmware_version_{0, 0, 0};
  std::queue<gamepad::input::State> inputs_queue_;
  gamepad::input::Tracker input_tracker_;
  cyf::HysteresisQuantizer<int32_t> rssi_quantizer_;
  uint64_t last_update_tx_power_ms_{0};
  bool dynamic_tx_power_enabled_ = true;
  int32_t local_tx_power_{0};
};
}  // namespace codex_pad