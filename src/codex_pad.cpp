#include "codex_pad.h"

#include "esp_arduino_version.h"

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 3, 0) && ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(3, 3, 2)
// clang-format off
#error "This library is incompatible with ESP-Arduino versions 3.3.0 to 3.3.2 (inclusive) due to known bugs. Please use version 3.3.3 or later, or downgrade to a version before 3.3.0."
#error "此库与 ESP-Arduino 版本 3.3.0 到 3.3.2（包含）不兼容，因为存在已知错误。请使用 3.3.3 或更高版本，或降级到 3.3.0 之前的版本。"
// clang-format on
#endif

namespace {
const BLEUUID kGapServiceUuid(static_cast<uint16_t>(0x1800));
const BLEUUID kGapDeviceNameUuid(static_cast<uint16_t>(0x2A00));

const BLEUUID kInputsServiceUuid(static_cast<uint16_t>(0xFFA0));
const BLEUUID kInputsCharacteristicUuid(static_cast<uint16_t>(0xFFA1));

const BLEUUID kBatteryServiceUuid(static_cast<uint16_t>(0x180F));
const BLEUUID kBatteryLevelCharacteristicUuid(static_cast<uint16_t>(0x2A19));

const BLEUUID kDeviceInfoServiceUuid(static_cast<uint16_t>(0x180A));
const BLEUUID kModelNumberCharacteristicUuid(static_cast<uint16_t>(0x2A24));
const BLEUUID kSerialNumberCharacteristicUuid(static_cast<uint16_t>(0x2A25));
const BLEUUID kFirmwareRevisionCharacteristicUuid(static_cast<uint16_t>(0x2A26));
const BLEUUID kManufacturerNameCharacteristicUuid(static_cast<uint16_t>(0x2A29));
}  // namespace

void CodexPad::Init() {
  if (ble_client_ != nullptr) {
    return;
  }
  if (!BLEDevice::getInitialized()) {
    BLEDevice::init("");
  }
  ble_client_ = BLEDevice::createClient();
}

bool CodexPad::Connect(const std::string& mac_address) {
  // log_i("Connect %s", mac_address.c_str());
  if (ble_client_ == nullptr) {
    return false;
  }

  ble_client_->disconnect();
  const auto ret = ble_client_->connect(BLEAddress(mac_address.c_str()));
  if (!ret) {
    // log_e("connect failed");
    return ret;
  }

  device_name_ = ble_client_->getValue(kGapServiceUuid, kGapDeviceNameUuid).c_str();

  const auto device_info_service = ble_client_->getService(kDeviceInfoServiceUuid);
  if (device_info_service != nullptr) {
    if (auto characteristic = device_info_service->getCharacteristic(kModelNumberCharacteristicUuid); characteristic != nullptr) {
      model_number_ = characteristic->readValue().c_str();
    }
    if (auto characteristic = device_info_service->getCharacteristic(kFirmwareRevisionCharacteristicUuid); characteristic != nullptr) {
      const auto firmware_revision = characteristic->readValue();
      for (uint8_t i = 0; i < sizeof(device_firmware_version_); i++) {
        device_firmware_version_[i] = firmware_revision[i];
      }
    }
  } else {
    goto Failed;
  }

  if (auto service = ble_client_->getService(kInputsServiceUuid); service != nullptr) {
    if (auto characteristic = service->getCharacteristic(kInputsCharacteristicUuid); characteristic != nullptr) {
      if (characteristic->canNotify()) {
        characteristic->registerForNotify(
            std::bind(&CodexPad::OnNotify, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), true);
      } else {
        goto Failed;
      }
    } else {
      goto Failed;
    }
  } else {
    goto Failed;
  }

  return ret;

Failed:
  if (ble_client_ != nullptr) {
    ble_client_->disconnect();
  }
  return false;
}

bool CodexPad::is_connected() {
  if (ble_client_ == nullptr) {
    return false;
  }

  return ble_client_->isConnected();
}

void CodexPad::set_tx_power(const CodexPad::TxPower tx_power) {
  if (ble_client_ != nullptr) {
    ble_client_->getService(BLEUUID(static_cast<uint16_t>(0x1804)))
        ->getCharacteristic(BLEUUID(static_cast<uint16_t>(0x2A07)))
        ->writeValue(static_cast<uint8_t>(tx_power));
  }
}

std::vector<uint8_t> CodexPad::FetchInputs() {
  std::lock_guard<std::mutex> l(mutex_);
  return std::move(state_);
}

void CodexPad::OnNotify(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t length, bool is_notify) {
  std::lock_guard<std::mutex> l(mutex_);
  if (characteristic != nullptr && characteristic->getUUID().equals(kInputsCharacteristicUuid)) {
    state_ = std::vector<uint8_t>(data, data + length);
  }
}