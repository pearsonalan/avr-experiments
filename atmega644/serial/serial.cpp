//
// This Blink program is for the ATmega644pa with a 20Mhz external crystal
// oscillator
//
// Wiring:
//   PA0 (pin 40) -> LED -> 220R -> GND
//

#include <Arduino.h>

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("Setup complete");
}

// the loop function runs over and over again forever
int iteration = 0;
void loop() {
  Serial.print("iteration ");
  Serial.println(iteration++);
  delay(1000);
}
