/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

#include <Arduino.h>

// the setup function runs once when you press reset or power the board
void setup() {
  DDRA = (1<<DDA0);
  PORTA = 0;
}

// the loop function runs over and over again forever
void loop() {
  PINA = 1;
  delay(100);                       // wait for a second
  PINA = 0;
  delay(100);                       // wait for a second
}
