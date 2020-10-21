// Blink2
//
// Blinks Two LEDs Simultaneously.  Based on Blink2 tutorial from 
// http://freenove.com.
//

#include <Arduino.h>

constexpr int LED1 = 4;   // Pin assignment for LED 1
constexpr int LED2 = 5;   // Pin assignment for LED 2

// Setup is run only once at the start of the program
void setup() {
  // Configure both LED pins as output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // Set up Serial output
  Serial.begin(115200);
}

// The loop function runs repeatedly
void loop() {
  Serial.println("LED STATE 0");
  digitalWrite(LED1, HIGH);   // turn the LED 1 on
  digitalWrite(LED2, LOW);    // turn the LED 2 off
  delay(1000);

  Serial.println("LED STATE 1");
  digitalWrite(LED1, LOW);    // turn the LED 1 off
  digitalWrite(LED2, HIGH);   // turn the LED 2 on
  delay(1000);
}
