#include <Arduino.h>

// Display digits on a seven segment display using a shift register
class SevenSegment {
 public:
  SevenSegment(int latch_pin, int data_pin, int clock_pin) :
      latch_pin_(latch_pin), data_pin_(data_pin), clock_pin_(clock_pin) {
    pinMode(latch_pin_, OUTPUT);
    pinMode(data_pin_, OUTPUT);
    pinMode(clock_pin_, OUTPUT);
  }

  void display(int pattern);
  void displayAndWait(int pattern, int delay_time);
  void displayAndFlash(int pattern);

private:
  int latch_pin_;
  int data_pin_;
  int clock_pin_;
};

void SevenSegment::display(int pattern) {
  digitalWrite(latch_pin_, LOW);
  shiftOut(data_pin_, clock_pin_, MSBFIRST, pattern);
  digitalWrite(latch_pin_, HIGH);
}

void SevenSegment::displayAndWait(int pattern, int delay_time) {
  display(pattern);
  delay(delay_time);
}

void SevenSegment::displayAndFlash(int pattern) {
  for (int i = 0; i < 2; i++ ) {
    displayAndWait(pattern, 250);
    displayAndWait(pattern & 0x7F, 250);
  }
}

int main() {
  init();

  SevenSegment display(2, 3, 4);

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
  
  for (;;) {
    display.displayAndFlash(0xC0);
    display.displayAndFlash(0xF9);
    display.displayAndFlash(0xA4);
    display.displayAndFlash(0xB0);
    display.displayAndFlash(0x99);
    display.displayAndFlash(0x92);
    display.displayAndFlash(0x82);
    display.displayAndFlash(0xF8);
    display.displayAndFlash(0x80);
    display.displayAndFlash(0x90);
  }
}
