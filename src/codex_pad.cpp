#include "codex_pad.h"

#include <memory>

#include "WString.h"
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

bool HasAxisValueChangedSignificantly(const int16_t prev_value, const int16_t current_value, const uint8_t threshold) {
  return prev_value != current_value && (current_value == 0 || current_value == 255 || std::abs(current_value - prev_value) >= threshold);
}

}  // namespace

CodexPad::CodexPad() {
}

CodexPad::~CodexPad() {
  if (ble_client_ != nullptr) {
    ble_client_->disconnect();
    delete ble_client_;
  }
}

void CodexPad::Init() {
  if (!BLEDevice::getInitialized()) {
    BLEDevice::init("");
  }
}

bool CodexPad::Connect(const std::string& mac_address, const uint32_t timeout_ms) {
  return Connect(BLEAddress(mac_address.c_str()), timeout_ms);
}

bool CodexPad::ScanAndConnect(const uint32_t button_mask) {
  auto scanner = BLEDevice::getScan();
  scanner->setActiveScan(true);  // active scan uses more power, but get results faster
  scanner->setInterval(1000);
  scanner->setWindow(999);  // less or equal setInterval value
#if ESP_ARDUINO_VERSION_MAJOR <= 2
  auto scan_results = scanner->start(1, false);
  auto result = &scan_results;
#else
  auto result = scanner->start(1, false);
#endif
  int32_t rssi = INT16_MIN;
  std::unique_ptr<BLEAddress> mac_address;

#pragma pack(push, 1)
  struct alignas(1) ManufacturerData {
    uint16_t company_id = 0xFFFF;
    uint8_t header[8] = {'C', 'o', 'd', 'e', 'x', 'P', 'a', 'd'};
    uint8_t version_major = 0;
    uint8_t version_minor = 0;
    uint8_t version_patch = 0;
    uint32_t button_state = 0;
  };
#pragma pack(pop)

  if (result != nullptr) {
    for (size_t i = 0; i < result->getCount(); i++) {
      auto device = result->getDevice(i);
      if (device.haveName() &&
#if ESP_ARDUINO_VERSION_MAJOR <= 2
          String(device.getName().c_str()).startsWith("CodexPad-")
#else
          device.getName().startsWith("CodexPad-")
#endif
          && device.haveRSSI() && device.haveManufacturerData()) {
        const auto manufacturer_data = device.getManufacturerData();
        if (manufacturer_data.length() >= sizeof(ManufacturerData)) {
          const auto data = reinterpret_cast<const ManufacturerData*>(manufacturer_data.c_str());
          if (data->company_id == 0xFFFF && memcmp(data->header, "CodexPad", 8) == 0 && data->button_state == button_mask &&
              device.getRSSI() > rssi) {
            rssi = device.getRSSI();
#if __cplusplus >= 201402L
            mac_address = std::make_unique<BLEAddress>(device.getAddress());
#else
            mac_address = std::unique_ptr<BLEAddress>(new BLEAddress(device.getAddress()));
#endif
          }
        }
      }
    }
  }

  scanner->clearResults();

  return mac_address ? Connect(*mac_address, 2000) : false;
}

void CodexPad::Update() {
  if (ble_client_ == nullptr) {
    return;
  }

  const auto current_connection_state = ble_client_->isConnected();
  if (connected_ != current_connection_state) {
    connected_ = current_connection_state;
    if (!connected_) {
      Reset();
      return;
    }
  }

  if (!connected_) {
    return;
  }

  prev_inputs_ = current_inputs_;
  do {
    std::lock_guard<std::mutex> l(mutex_);
    if (inputs_queue_.empty()) {
      break;
    }
    current_inputs_ = std::move(inputs_queue_.front());
    inputs_queue_.pop();
  } while (false);
}

void CodexPad::set_tx_power(const CodexPad::TxPower tx_power) {
  if (ble_client_ != nullptr) {
    ble_client_->getService(BLEUUID(static_cast<uint16_t>(0x1804)))
        ->getCharacteristic(BLEUUID(static_cast<uint16_t>(0x2A07)))
        ->writeValue(static_cast<uint8_t>(tx_power));
  }
}

bool CodexPad::pressed(const Button button) const {
  return (prev_inputs_.button_states & static_cast<uint32_t>(button)) == 0 && (current_inputs_.button_states & static_cast<uint32_t>(button)) != 0;
}

bool CodexPad::released(const Button button) const {
  return (prev_inputs_.button_states & static_cast<uint32_t>(button)) != 0 && (current_inputs_.button_states & static_cast<uint32_t>(button)) == 0;
}

bool CodexPad::holding(const Button button) const {
  return (prev_inputs_.button_states & static_cast<uint32_t>(button)) != 0 && (current_inputs_.button_states & static_cast<uint32_t>(button)) != 0;
}

bool CodexPad::button_state(const Button button) const {
  return (current_inputs_.button_states & static_cast<uint32_t>(button)) != 0;
}

uint32_t CodexPad::button_states() const {
  return current_inputs_.button_states;
}

uint8_t CodexPad::axis_value(const Axis axis) const {
  return current_inputs_.axis_values[static_cast<size_t>(axis)];
}

std::array<uint8_t, CodexPad::kAxisValueNum> CodexPad::axis_values() const {
  std::array<uint8_t, kAxisValueNum> axis_values = {0x80, 0x80, 0x80, 0x80};
  for (size_t i = 0; i < kAxisValueNum; i++) {
    axis_values[i] = current_inputs_.axis_values[i];
  }
  return axis_values;
}

bool CodexPad::HasAxisValueChanged(const Axis axis, const uint8_t threshold) const {
  return HasAxisValueChangedSignificantly(
      current_inputs_.axis_values[static_cast<size_t>(axis)], prev_inputs_.axis_values[static_cast<size_t>(axis)], threshold);
}

bool CodexPad::Connect(const BLEAddress& address, [[maybe_unused]] const uint32_t timeout_ms) {
  Reset();
  BLERemoteService* device_info_service = nullptr;
  ble_client_ = BLEDevice::createClient();

#if ESP_ARDUINO_VERSION_MAJOR <= 2
  const auto ret = ble_client_->connect(address, BLE_ADDR_TYPE_PUBLIC);
#else
#ifdef BLE_ADDR_PUBLIC
  const auto ret = ble_client_->connect(address, BLE_ADDR_PUBLIC, timeout_ms == UINT32_MAX ? portMAX_DELAY : timeout_ms);
#else
  const auto ret = ble_client_->connect(address, BLE_ADDR_TYPE_PUBLIC, timeout_ms == UINT32_MAX ? portMAX_DELAY : timeout_ms);
#endif
#endif

  if (!ret) {
    goto Failed;
  }

  device_name_ = ble_client_->getValue(kGapServiceUuid, kGapDeviceNameUuid).c_str();
  device_info_service = ble_client_->getService(kDeviceInfoServiceUuid);
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

  connected_ = ble_client_->isConnected();
  return ret;

Failed:
  if (ble_client_ != nullptr) {
    ble_client_->disconnect();
    delete ble_client_;
    ble_client_ = nullptr;
  }
  connected_ = false;
  return false;
}

void CodexPad::OnNotify(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t length, bool is_notify) {
  if (characteristic != nullptr && characteristic->getUUID().equals(kInputsCharacteristicUuid)) {
    if (length != sizeof(Inputs)) {
      printf("WARNING: length != sizeof(Inputs)\n");
      return;
    }

    std::lock_guard<std::mutex> l(mutex_);
    if (inputs_queue_.size() > kInputsQueueMax) {
      inputs_queue_.pop();
    }
    Inputs inputs;
    memcpy(&inputs, data, sizeof(inputs));
    inputs_queue_.emplace(std::move(inputs));
  }
}

void CodexPad::Reset() {
  if (ble_client_ != nullptr) {
    ble_client_->disconnect();
    delete ble_client_;
    ble_client_ = nullptr;
  }

  device_name_.clear();
  model_number_.clear();
  memset(device_firmware_version_, 0, sizeof(device_firmware_version_));
  connected_ = false;
  prev_inputs_ = {};
  current_inputs_ = {};
  std::lock_guard<std::mutex> l(mutex_);
  inputs_queue_ = {};
}