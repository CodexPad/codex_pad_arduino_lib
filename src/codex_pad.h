#pragma once

#include <BLEDevice.h>

#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <vector>

class CodexPad {
 public:
  static constexpr size_t kAxisValueNum = 4;

  enum class TxPower : int8_t {
    kMinus16dBm = -16,
    kMinus12dBm = -12,
    kMinus8dBm = -8,
    kMinus5dBm = -5,
    kMinus3dBm = -3,
    k0dBm = 0,
    k1dBm = 1,
    k2dBm = 2,
    k3dBm = 3,
    k4dBm = 4,
    k5dBm = 5,
    k6dBm = 6
  };

  enum Button : uint32_t {
    kUp = static_cast<uint32_t>(1) << 0,
    kDown = static_cast<uint32_t>(1) << 1,
    kLeft = static_cast<uint32_t>(1) << 2,
    kRight = static_cast<uint32_t>(1) << 3,
    kSquareX = static_cast<uint32_t>(1) << 4,
    kTriangleY = static_cast<uint32_t>(1) << 5,
    kCrossA = static_cast<uint32_t>(1) << 6,
    kCircleB = static_cast<uint32_t>(1) << 7,
    kL1 = static_cast<uint32_t>(1) << 8,
    kL2 = static_cast<uint32_t>(1) << 9,
    kL3 = static_cast<uint32_t>(1) << 10,
    kR1 = static_cast<uint32_t>(1) << 11,
    kR2 = static_cast<uint32_t>(1) << 12,
    kR3 = static_cast<uint32_t>(1) << 13,
    kSelect = static_cast<uint32_t>(1) << 14,
    kStart = static_cast<uint32_t>(1) << 15,
    kHome = static_cast<uint32_t>(1) << 16,
  };

  enum Axis : size_t {
    kLeftStickX,
    kLeftStickY,
    kRightStickX,
    kRightStickY,
  };

  CodexPad();
  ~CodexPad() = default;
  void Init();
  bool Connect(const std::string& mac_address);
  void Update();
  bool is_connected();
  void set_tx_power(const TxPower power);

  inline const std::string& device_name() const {
    return device_name_;
  }

  inline const std::string& model_number() const {
    return model_number_;
  }

  bool pressed(const Button button) const;
  bool released(const Button button) const;
  bool holding(const Button button) const;
  bool button_state(const Button button) const;
  uint32_t button_states() const;
  uint8_t axis_value(const Axis axis) const;
  std::array<uint8_t, kAxisValueNum> axis_values() const;
  bool HasAxisValueChanged(const Axis axis, const uint8_t threshold) const;

 private:
  static constexpr size_t kButtonNum = 32;
  static constexpr size_t kInputsQueueMax = 10;

  struct Inputs {
    uint32_t button_states = 0;
    uint8_t axis_values[kAxisValueNum] = {0x80, 0x80, 0x80, 0x80};
  };

  std::optional<Inputs> FetchInputs();
  void OnNotify(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t length, bool is_notify);

  std::mutex mutex_;
  BLEClient* ble_client_ = nullptr;
  std::string device_name_;
  std::string model_number_;
  uint8_t device_firmware_version_[3] = {0};
  Inputs prev_inputs_;
  Inputs current_inputs_;
  std::queue<Inputs> inputs_queue_;
};