#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <Arduino.h>

#include "display.h"
#include "input-reader.h"
#include "radio.h"

class Controller {
 public:
  Controller(Display& display) : display_(display) {}

  void Run(InputReader& input);

  void SetActiveFrequency(const RadioFrequency& f);
  void SetStandbyFrequency(const RadioFrequency& f);

 private:
  long values_[2] = {0};

  Display& display_;
};

#endif