#pragma once

#include <BLEDevice.h>

#include <mutex>
#include <optional>
#include <string>

class CodexPad {
 public:
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

  CodexPad() = default;
  virtual ~CodexPad() = default;
  void Init();
  virtual bool Connect(const std::string& mac_address);
  bool is_connected();
  void set_tx_power(const TxPower power);

  inline const std::string& device_name() const {
    return device_name_;
  }

  inline const std::string& model_number() const {
    return model_number_;
  }

 protected:
  std::vector<uint8_t> FetchInputs();

 private:
  void OnNotify(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t length, bool is_notify);

  std::mutex mutex_;
  BLEClient* ble_client_ = nullptr;
  std::string device_name_;
  std::string model_number_;
  uint8_t device_firmware_version_[3] = {0};
  std::vector<uint8_t> state_;
};