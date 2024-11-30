#include <Arduino.h>

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(A0, OUTPUT);
}

constexpr int pins[] = {8, 9, 10, 11, 12, A0};
constexpr int kPinCount = 6;

static int p = 0;

void loop() {
  for (int i = 0; i < kPinCount; i++) {
    digitalWrite(pins[i], i == p ? LOW : HIGH);
  }
  delay(1200);
  p++;
  if (p == kPinCount) p = 0;
}