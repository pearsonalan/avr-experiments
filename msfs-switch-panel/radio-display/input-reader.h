#ifndef _INPUT_READER_H_
#define _INPUT_READER_H_

#include <Arduino.h>
#include "radio.h"

class InputReader {
 public:
  InputReader(Radio& radio) : radio_(radio) {}

  void CheckSerial();

 private:
  Radio& radio_;
};

#endif
