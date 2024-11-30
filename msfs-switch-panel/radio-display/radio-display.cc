#include <Arduino.h>
#include "display.h"
#include "radio.h"
#include "input-reader.h"
#include "controller.h"

// Pin connected to RCLK (Storage Register Clock or latch pin) of 74HC595
constexpr int LATCH_PIN = 10;

// Pin connected to SRCLK (Shift Register Clock) of 74HC595
constexpr int CLOCK_PIN = 9;

// Pin connected to SER of 74HC595
constexpr int DATA_PIN = 8;

int main() {
  init();

  Serial.begin(115200);
  while (!Serial)
    ;

  Display display(true, DATA_PIN, CLOCK_PIN, LATCH_PIN);
  Controller controller(display);
  Radio radio(controller);
  InputReader input(radio);
  controller.Run(input);
}
