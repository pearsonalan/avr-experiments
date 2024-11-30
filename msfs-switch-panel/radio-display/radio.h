#ifndef _RADIO_H_
#define _RADIO_H_

#include <Arduino.h>

class Controller;

struct RadioFrequency {
  int int_part;
  int dec_part;
};

constexpr int kDecimalStep = 25;
constexpr int kMinIntFreq = 118;
constexpr int kMaxIntFreq = 135;

enum ActiveInputPart {
  kIntegerPartActive,
  kDecimalPartActive
};

class Radio {
 public:
  Radio(Controller& controller);

  // Increment the decimal or integer part of the standby frequency
  // depending on which one is active.
  void Up();

  // Increment the decimal or integer part of the standby frequency
  // depending on which one is active.
  void Down();

  // Toggle whether the decimal or integer part is active for imput.
  void Toggle();

  // Swap the standby and active frequencies.
  void Swap();

 private:
  // Invoked to update the controller with the frequencies.
  void UpdateController();

  Controller& controller_;

  ActiveInputPart active_part_ = kIntegerPartActive;

  RadioFrequency active_freq_ = {kMinIntFreq, 0};
  RadioFrequency standby_freq_ = {kMinIntFreq, 0};
};

#endif