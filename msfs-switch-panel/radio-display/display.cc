#include "display.h"

// Encoding of digits 0-9 on a Common Cathode seven-segment display:
//
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

// This defines the patterns to write to the shift register to
// show the digits 0-9 as derived from the table above.
constexpr int patterns[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                              0x6D, 0x7D, 0x07, 0x7F, 0x6F};

constexpr int char_pins[6] = {PC2, PC1, PC0, PC3, PC4, PC5};

#define BV(x) _BV(x)

Display::Display(bool leading_zeros, int data_pin, int clock_pin, int latch_pin)
    : leading_zeros_(leading_zeros),
      data_pin_(data_pin),
      clock_pin_(clock_pin),
      latch_pin_(latch_pin) {
  // set pins to output so you can control the shift register
  pinMode(latch_pin_, OUTPUT);
  pinMode(clock_pin_, OUTPUT);
  pinMode(data_pin_, OUTPUT);

  // Set output direction for PC0..PC5 to out by setting bits
  // DDRC[5:0] to 1
  DDRC = 0x3F;

  // Set all characters off.  We need to drive the character enable pin low
  // to allow current to flow through the LEDs
  PORTC |= 0x3F;
}

void Display::display1(unsigned long value) {
  for (uint8_t i = 0; i < 6; i++) {
    show_char_1_[5 - i] = (value != 0 || leading_zeros_);
    digits_1_[5 - i] = (uint8_t)(value % 10);
    value = value / 10;
  }
}

void Display::display2(unsigned long value) {
  for (uint8_t i = 0; i < 6; i++) {
    show_char_2_[5 - i] = (value != 0 || leading_zeros_);
    digits_2_[5 - i] = (uint8_t)(value % 10);
    value = value / 10;
  }
}

unsigned long shift_time = 0;

void Display::refresh() {
  // Turn off the character that was active (set enable pin HIGH)
  PORTC |= BV(char_pins[active_character_]);

  // advance to the next character
  if (++active_character_ == 6) active_character_ = 0;

  // load the patterns to show for the active character
  int pattern1 = patterns[digits_1_[active_character_]];
  if (!show_char_1_[active_character_]) pattern1 = 0;
  if (show_dp_1_[active_character_]) pattern1 |= 0x80;

  int pattern2 = patterns[digits_2_[active_character_]];
  if (!show_char_2_[active_character_]) pattern2 = 0;
  if (show_dp_2_[active_character_]) pattern2 |= 0x80;

  unsigned long now = micros();

  // Set the latchPin low so the LEDs don't change while sending in bits
  digitalWrite(latch_pin_, LOW);

  // Shift out the bits
  shiftOut(data_pin_, clock_pin_, MSBFIRST, pattern2);
  shiftOut(data_pin_, clock_pin_, MSBFIRST, pattern1);

  // Set the latch pin high so the LEDs will light up
  digitalWrite(latch_pin_, HIGH);

  shift_time += (micros() - now);

  // Drive the character enable pin low to turn on the character
  PORTC &= ~BV(char_pins[active_character_]);
}
