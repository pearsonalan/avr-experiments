#include <Arduino.h>

int latchPin = 2;  // connect to pin 12 on the 74HC595
int dataPin = 3;   // connect to pin 14 on the 74HC595
int clockPin = 4;  // connect to pin 11 on the 74HC595

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void display(int pattern) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, pattern);
  digitalWrite(latchPin, HIGH);
}

void displayAndWait(int pattern, int delay_time) {
  display(pattern);
  delay(delay_time);
}

void displayAndFlash(int pattern) {
  for (int i = 0; i < 2; i++ ) {
    displayAndWait(pattern, 250);
    displayAndWait(pattern & 0x7F, 250);
  }
}

void loop() {
  // DIGIT  SEGMENTS       PGFE DCBA  HEX 
  // 0      A,B,C,D,E,F    1100 0000  0xC0
  // 1      B,C            1111 1001  0xF9
  // 2      A,B,D,E,G      1010 0100  0xA4
  // 3      A,B,C,D,G      1011 0000  0xB0
  // 4      B,C,F,G        1001 1001  0x99
  // 5      A,C,D,F,G      1001 0010  0x92
  // 6      A,C,D,E,F,G    1000 0010  0x82
  // 7      A,B,C          1111 1000  0xF8
  // 8      A,B,C,D,E,F,G  1000 0000  0x80
  // 9      A,B,C,D,F,G    1001 0000  0x90
  
  displayAndFlash(0xC0);
  displayAndFlash(0xF9);
  displayAndFlash(0xA4);
  displayAndFlash(0xB0);
  displayAndFlash(0x99);
  displayAndFlash(0x92);
  displayAndFlash(0x82);
  displayAndFlash(0xF8);
  displayAndFlash(0x80);
  displayAndFlash(0x90);
}
