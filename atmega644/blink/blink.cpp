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
  DDRA = (1<<DDA0);
  PORTA = 0;
}

// the loop function runs over and over again forever
void loop() {
  PINA = 1;
  delay(500);
  PINA = 0;
  delay(500);
}
