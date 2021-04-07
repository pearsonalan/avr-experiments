#include "radio.h"
#include "controller.h"

Radio::Radio(Controller& controller) : controller_(controller) {
  UpdateController();
}

void Radio::Up() {
  if (active_part_ == kIntegerPartActive) {
    standby_freq_.int_part++;
    if (standby_freq_.int_part > kMaxIntFreq) {
      standby_freq_.int_part = kMinIntFreq;
    }
  } else {
    standby_freq_.dec_part += kDecimalStep;
    if (standby_freq_.dec_part >= 1000) {
      standby_freq_.dec_part = 0;
    }
  }
  UpdateController();
}

void Radio::Down() {
  if (active_part_ == kIntegerPartActive) {
    standby_freq_.int_part--;
    if (standby_freq_.int_part < kMinIntFreq) {
      standby_freq_.int_part = kMaxIntFreq;
    }
  } else {
    standby_freq_.dec_part -= kDecimalStep;
    if (standby_freq_.dec_part < 0) {
      standby_freq_.dec_part = 1000 - kDecimalStep;
    }
  }
  UpdateController();
}

void Radio::Toggle() {
  if (active_part_ == kIntegerPartActive) {
    active_part_ = kDecimalPartActive;
  } else {
    active_part_ = kIntegerPartActive;
  }
  UpdateController();
}

void Radio::Swap() {
  RadioFrequency tmp;
  tmp = standby_freq_;
  standby_freq_ = active_freq_;
  active_freq_ = tmp;
  UpdateController();
}

void Radio::UpdateController() {
  controller_.SetActiveFrequency(active_freq_);
  controller_.SetStandbyFrequency(standby_freq_);
}
