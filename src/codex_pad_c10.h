#pragma once

#include "codex_pad.h"

class CodexPadC10 : public CodexPad {
 public:
  enum class Button : uint8_t {
    kL3 = static_cast<uint8_t>(1) << 0,
    kStart = static_cast<uint8_t>(1) << 1,
    kX = static_cast<uint8_t>(1) << 2,
    kY = static_cast<uint8_t>(1) << 3,
    kA = static_cast<uint8_t>(1) << 4,
    kB = static_cast<uint8_t>(1) << 5,
  };

  enum class Axis : uint8_t {
    kLeftStickX,
    kLeftStickY,
  };

  CodexPadC10() = default;
  virtual ~CodexPadC10() = default;

  bool Connect(const std::string& mac_address) override;
  void Update();
  bool pressed(const Button button) const;
  bool released(const Button button) const;
  bool holding(const Button button) const;
  bool button_state(const Button button) const;
  uint8_t button_states() const;
  uint8_t axis_value(const Axis axis) const;
  const uint8_t* axis_values() const;
  bool HasAxisValueChanged(const Axis axis, const uint8_t threshold) const;

 private:
  bool HasAxisValueChangedSignificantly(const int16_t prev_value, const int16_t current_value, const uint8_t threshold) const;
  std::vector<uint8_t> prev_inputs_{0, 127, 127};
  std::vector<uint8_t> current_inputs_{0, 127, 127};
};