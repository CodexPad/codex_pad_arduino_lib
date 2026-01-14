#include "codex_pad_c10.h"

#include <esp32-hal-log.h>

#include <cstdio>

bool CodexPadC10::Connect(const std::string& mac_address) {
  if (CodexPad::Connect(mac_address)) {
    if (model_number() != "C10") {
      printf("FATAL: The Model number of remote device is %s not C10, please check\n", device_name().c_str());
      abort();
      return false;
    }
    return true;
  }
  return false;
}

void CodexPadC10::Update() {
  prev_inputs_ = current_inputs_;
  auto new_inputs = FetchInputs();

  if (!new_inputs.empty()) {
    current_inputs_ = std::move(new_inputs);
  }
}

bool CodexPadC10::pressed(const CodexPadC10::Button button) const {
  return (prev_inputs_[0] & static_cast<uint8_t>(button)) == 0 && (current_inputs_[0] & static_cast<uint8_t>(button)) != 0;
}

bool CodexPadC10::released(const CodexPadC10::Button button) const {
  return (prev_inputs_[0] & static_cast<uint8_t>(button)) != 0 && (current_inputs_[0] & static_cast<uint8_t>(button)) == 0;
}

bool CodexPadC10::holding(const CodexPadC10::Button button) const {
  return (prev_inputs_[0] & static_cast<uint8_t>(button)) != 0 && (current_inputs_[0] & static_cast<uint8_t>(button)) != 0;
}

bool CodexPadC10::button_state(const Button button) const {
  return (current_inputs_[0] & static_cast<uint8_t>(button)) != 0;
}

uint8_t CodexPadC10::button_states() const {
  return current_inputs_[0];
}

uint8_t CodexPadC10::axis_value(const CodexPadC10::Axis axis) const {
  switch (axis) {
    case CodexPadC10::Axis::kLeftStickX: {
      return current_inputs_[1];
    }
    case CodexPadC10::Axis::kLeftStickY: {
      return current_inputs_[2];
    }
    default: {
      abort();
      return 0;
    }
  }
  return 0;
}

const uint8_t* CodexPadC10::axis_values() const {
  return current_inputs_.data() + 1;
}

bool CodexPadC10::HasAxisValueChanged(const Axis axis, const uint8_t threshold) const {
  switch (axis) {
    case CodexPadC10::Axis::kLeftStickX: {
      return HasAxisValueChangedSignificantly(prev_inputs_[1], current_inputs_[1], threshold);
    }
    case CodexPadC10::Axis::kLeftStickY: {
      return HasAxisValueChangedSignificantly(prev_inputs_[2], current_inputs_[2], threshold);
    }
    default: {
      abort();
      return false;
    }
  }
  return false;
}

bool CodexPadC10::HasAxisValueChangedSignificantly(const int16_t prev_value, const int16_t current_value, const uint8_t threshold) const {
  return prev_value != current_value && (current_value == 0 || current_value == 255 || std::abs(current_value - prev_value) >= threshold);
}
