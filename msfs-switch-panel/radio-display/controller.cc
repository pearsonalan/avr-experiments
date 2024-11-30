#include "controller.h"

inline long FrequencyToLong(const RadioFrequency& f) {
  return (long) f.int_part * 1000L + (long) f.dec_part;
}

void Controller::SetActiveFrequency(const RadioFrequency& f) {
  values_[0] = FrequencyToLong(f);
  display_.display1(values_[0]);
}

void Controller::SetStandbyFrequency(const RadioFrequency& f) {
  values_[1] = FrequencyToLong(f);
  display_.display2(values_[1]);
}

void Controller::Run(InputReader& input) {
  Serial.println("Ready!");

  display_.enableDP1(2);
  display_.enableDP2(2);

  int last = millis();
  for (;;) {
    int now = millis();

    input.CheckSerial();
    display_.refresh();
    delayMicroseconds(1000);
  }
}
