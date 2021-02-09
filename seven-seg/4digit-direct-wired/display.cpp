#include <Arduino.h>

// DIGIT  SEGMENTS       PGFE DCBA  HEX 
// 0      A,B,C,D,E,F    0011 1111  0x3F
// 1      B,C            0000 0110  0x06
// 2      A,B,D,E,G      0101 1011  0x5B
// 3      A,B,C,D,G      0100 1111  0x4F
// 4      B,C,F,G        0110 0110  0x66
// 5      A,C,D,F,G      0110 1101  0x6D
// 6      A,C,D,E,F,G    0111 1101  0x7D
// 7      A,B,C          0000 0111  0x07
// 8      A,B,C,D,E,F,G  0111 1111  0x7F
// 9      A,B,C,D,F,G    0110 1111  0x6F

// This defines the patterns to use on the DPORT to show the digits 0-9 as 
// derived from the table above.
constexpr int patterns[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

// Shows 4 digits on a 4-character 7 segment display.
// This code assumes the following wiring
//
// DISPLAY_PIN     DESCRIPTION       ATMEGA328 PIN 
// 1               Segment E         PD4
// 2               Segment D         PD3
// 3               Decimal Point     PD7
// 4               Segment C         PD2
// 5               Segment G         PD6
// 6               Digit 4           PB4
// 7               Segment B         PD1
// 8               Digit 3           PB3
// 9               Digit 2           PB2
// 10              Segment F         PD5
// 11              Segment A         PD0
// 12              Digit 1           PB1

class Display {
public:
  Display(bool leading_zeros);

  // Set the value (0-9999) to show on the 4-character display
  void display(int value);

  // Must be called approximately every 2ms to keep the Persistence-of-vision
  // effect showing all 4 characters.
  void refresh();

private:
  int active_character_ = 0;
  bool leading_zeros_ = false;
  int digits_[4] = {0};
  bool show_char_[4] = {false};
};

Display::Display(bool leading_zeros) : leading_zeros_(leading_zeros) {
  for (int i = 9; i <= 12; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  for (int i = 0; i <= 7; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
}

void Display::display(int value) {
  for (int i = 0; i < 4; i++) {
    show_char_[3 - i] = (value != 0 || leading_zeros_);
    digits_[3 - i] = value % 10;
    value = value / 10;
  }
}

void Display::refresh() {
  digitalWrite(active_character_ + 9, HIGH);
  if (++active_character_ == 4) active_character_ = 0;
  int pattern = patterns[digits_[active_character_]];
  if (!show_char_[active_character_]) pattern = 0;
  PORTD = pattern;
  digitalWrite(active_character_ + 9, LOW);
}

Display display(false);

static int val = 0;

void setup() {
  display.display(val);
}

void loop() {
  static int n = 0;

  if (++n == 50) {
    display.display(++val);
    n = 0;
  }

  display.refresh(); 
  delay(2);
}
