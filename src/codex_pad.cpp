#include "codex_pad.h"

#include <Arduino.h>

#include <memory>

#include "WString.h"

namespace {
constexpr uint16_t kGapServiceUuid{0x1800};
constexpr uint16_t kGapDeviceNameUuid{0x2A00};

constexpr uint16_t kInputsServiceUuid{0xFFA0};
constexpr uint16_t kInputsCharacteristicUuid{0xFFA1};

constexpr uint16_t kBatteryServiceUuid{0x180F};
constexpr uint16_t kBatteryLevelCharacteristicUuid{0x2A19};

constexpr uint16_t kDeviceInfoServiceUuid{0x180A};
constexpr uint16_t kModelNumberCharacteristicUuid{0x2A24};
constexpr uint16_t kSerialNumberCharacteristicUuid{0x2A25};
constexpr uint16_t kFirmwareRevisionCharacteristicUuid{0x2A26};
constexpr uint16_t kManufacturerNameCharacteristicUuid{0x2A29};
}  // namespace

CodexPad::CodexPad() noexcept {}

CodexPad::~CodexPad() noexcept { Reset(); }

void CodexPad::Init() noexcept {
  if (!NimBLEDevice::isInitialized()) {
    NimBLEDevice::init("CodexPadClient");
  }
}

bool CodexPad::Connect(const std::string& bluetooth_device_address, const uint32_t timeout_ms) noexcept {
  // check mac address is valid
  if (bluetooth_device_address.length() != 17 || bluetooth_device_address[2] != ':' || bluetooth_device_address[5] != ':' ||
      bluetooth_device_address[8] != ':' || bluetooth_device_address[11] != ':' || bluetooth_device_address[14] != ':') {
    abort();
    return false;
  }

  return Connect(NimBLEAddress(bluetooth_device_address, 0), false, timeout_ms);
}

bool CodexPad::ScanAndConnect(const gamepad::input::Button buttons) noexcept {
  auto scanner = NimBLEDevice::getScan();
  scanner->setActiveScan(true);  // active scan uses more power, but get results faster
  scanner->setInterval(1000);
  scanner->setWindow(999);  // less or equal setInterval value

  if (!scanner->start(1000)) {
    scanner->stop();
    scanner->clearResults();
    // printf("Scan failed\n");
    return false;
  }

  while (scanner->isScanning()) {
    delay(100);
  }

  // printf("Scan done, device count: %d\n", scanner->getResults().getCount());

  int8_t rssi = INT8_MIN;
  NimBLEAddress address;

#pragma pack(push, 1)
  struct ManufacturerSpecificData {
    uint16_t company_id = 0xFFFF;
    uint8_t header[8] = {'C', 'o', 'd', 'e', 'x', 'P', 'a', 'd'};
    uint8_t version_major = 0;
    uint8_t version_minor = 0;
    uint8_t version_patch = 0;
    uint32_t button_state = 0;
    uint8_t button_states_duration_seconds = 0;
  };
#pragma pack(pop)

  for (const auto device : scanner->getResults()) {
    if (device->haveName() && String(device->getName().c_str()).startsWith("CodexPad-") && device->haveManufacturerData()) {
      // printf("Name: %s\n", device->getName().c_str());
      const auto manufacturer_data = device->getManufacturerData();
      if (manufacturer_data.length() >= sizeof(ManufacturerSpecificData)) {
        const auto data = reinterpret_cast<const ManufacturerSpecificData*>(manufacturer_data.c_str());
        if (data->company_id == 0xFFFF                                                 // company id
            && memcmp(data->header, "CodexPad", 8) == 0                                // header
            && data->button_state == static_cast<uint32_t>(buttons)                    // button mask
            && device->getRSSI() > rssi                                                // rssi
            && (data->version_major < 2 || data->button_states_duration_seconds >= 1)  // button states duration
        ) {
          rssi = device->getRSSI();
          address = device->getAddress();
          // printf("Found device, rssi: %d, address: %s\n", rssi, address.toString().c_str());
        }
      }
    }
  }

  scanner->clearResults();

  return address.isNull() ? false : Connect(address, 2000);
}

const gamepad::input::Tracker& CodexPad::Update() noexcept {
  if (ble_client_ == nullptr) {
    return input_tracker_;
  }

  if (!ble_client_->isConnected()) {
    Reset();
    return input_tracker_;
  }

  input_tracker_.Tick();

  do {
    std::lock_guard<std::mutex> l(mutex_);
    if (inputs_queue_.empty()) {
      break;
    }

    input_tracker_.Update(inputs_queue_.front());
    inputs_queue_.pop();
  } while (false);

  return input_tracker_;
}

bool CodexPad::is_connected() const noexcept { return ble_client_ != nullptr && ble_client_->isConnected(); }

bool CodexPad::set_remote_tx_power(const CodexPad::TxPower tx_power) noexcept {
  if (ble_client_ == nullptr) {
    return false;
  }

  if (!ble_client_->isConnected()) {
    return false;
  }

  auto remote_service = ble_client_->getService(uint16_t{0x1804});
  if (remote_service == nullptr) {
    return false;
  }

  auto remote_characteristic = remote_service->getCharacteristic(uint16_t{0x2A07});
  if (remote_characteristic == nullptr) {
    return false;
  }

  return remote_characteristic->writeValue(static_cast<uint8_t>(tx_power));
}

bool CodexPad::Connect(const NimBLEAddress& address, bool async_connect, const uint32_t timeout_ms) {
  Reset();
  assert(ble_client_ == nullptr);
  ble_client_ = NimBLEDevice::createClient(address);
  ble_client_->setConnectTimeout(timeout_ms);
  auto ret = ble_client_->connect(true, async_connect, true);

  if (!ret || !ble_client_->isConnected()) {
    goto FAILED;
  }

  remote_device_name_ = ble_client_->getValue(kGapServiceUuid, kGapDeviceNameUuid);
  remote_model_number_ = ble_client_->getValue(kDeviceInfoServiceUuid, kModelNumberCharacteristicUuid);
  {
    auto firmware_revision = ble_client_->getValue(kDeviceInfoServiceUuid, kFirmwareRevisionCharacteristicUuid);
    if (firmware_revision.length() == remote_firmware_version_.size()) {
      memcpy(remote_firmware_version_.data(), firmware_revision.data(), firmware_revision.length());
    }
  }

  {
    auto remote_service = ble_client_->getService(kInputsServiceUuid);
    if (remote_service == nullptr) {
      goto FAILED;
    }

    auto remote_characteristic = remote_service->getCharacteristic(kInputsCharacteristicUuid);
    if (remote_characteristic == nullptr) {
      goto FAILED;
    }

    if (!remote_characteristic->canNotify()) {
      goto FAILED;
    }

    if (!remote_characteristic->subscribe(
            true, std::bind(&CodexPad::OnNotify, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                            std::placeholders::_4))) {
      goto FAILED;
    }
  }

  return ret;

FAILED:
  Reset();
  return false;
}

void CodexPad::OnNotify(const NimBLERemoteCharacteristic* remote_characteristic, const uint8_t* data, const size_t length,
                        const bool is_notify) {
  if (remote_characteristic != nullptr && remote_characteristic->getUUID().equals(kInputsCharacteristicUuid)) {
    if (length != sizeof(gamepad::input::State)) {
      printf("WARNING: length != sizeof(Inputs)\n");
      return;
    }

    std::lock_guard<std::mutex> l(mutex_);
    if (inputs_queue_.size() > kInputsQueueMax) {
      inputs_queue_.pop();
    }
    inputs_queue_.emplace(gamepad::input::State::FromBytes(data));
  }
}

void CodexPad::Reset() {
  if (ble_client_ != nullptr) {
    ble_client_->cancelConnect();
    ble_client_->disconnect();
    NimBLEDevice::deleteClient(ble_client_);
    ble_client_ = nullptr;
  }

  remote_device_name_.clear();
  remote_model_number_.clear();
  remote_firmware_version_.fill(0);
  input_tracker_.Reset();
  std::lock_guard<std::mutex> l(mutex_);
  inputs_queue_ = {};
}