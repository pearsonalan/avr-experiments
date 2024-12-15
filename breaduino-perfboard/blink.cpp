#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Initializing...");
  pinMode(13, OUTPUT);
}

int iteration = 0;

void loop() {
  Serial.print("Looping... iteration ");
  Serial.println(iteration);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  iteration++;
}
