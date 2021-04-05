#include <Arduino.h>

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
constexpr int patterns[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

// Pin connected to ST_CP of 74HC595
constexpr int latchPin = 8;

// Pin connected to SH_CP of 74HC595
constexpr int clockPin = 12;

// Pin connected to DS of 74HC595
constexpr int dataPin = 11;

constexpr int char_pins[6] = {
  PC0, PC1, PC2, PC3, PC4, PC5
};

#define BV(x) _BV(x)

// Class to manage a 6-digit seven segment display
//
// Character control signals for each of the 6 digits are attached to
// PC0..PC5.  Control signals for the seven segments are attached to
// the output pins of a shift register.
class Display {
public:
  Display(bool leading_zeros);

  // Set the value (0-999999) to show on the 4-character display
  void display1(unsigned long value);
  void display2(unsigned long value);

  // Sets the decimal point at the given character on
  void enableDP1(int c) { show_dp_1_[c] = true; }
  void enableDP2(int c) { show_dp_2_[c] = true; }

  // Turns off the decimal point at the given character on
  void disableDP1(int c) { show_dp_1_[c] = false; }
  void disableDP2(int c) { show_dp_2_[c] = false; }

  // Must be called approximately every 2ms to keep the Persistence-of-vision
  // effect showing all 6 characters.
  void refresh();

private:
  uint8_t active_character_ = 0;
  bool leading_zeros_ = false;

  uint8_t digits_1_[6] = {0};
  bool show_char_1_[6] = {false};
  bool show_dp_1_[6] = {false};

  uint8_t digits_2_[6] = {0};
  bool show_char_2_[6] = {false};
  bool show_dp_2_[6] = {false};
};

Display::Display(bool leading_zeros) : leading_zeros_(leading_zeros) {
  // set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

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
  digitalWrite(latchPin, LOW);

  // Shift out the bits
  shiftOut(dataPin, clockPin, MSBFIRST, pattern2);
  shiftOut(dataPin, clockPin, MSBFIRST, pattern1);

  // Set the latch pin high so the LEDs will light up
  digitalWrite(latchPin, HIGH);

  shift_time += (micros() - now);

  // Drive the character enable pin low to turn on the character
  PORTC &= ~BV(char_pins[active_character_]);
}


int main() {
  init();

  Display display(true);

  unsigned int value = 0;
  display.display1(value);
  display.display2(0);
  display.enableDP1(4);

  int last_update = millis();
  uint8_t iters = 0;
  for (;;) {
    int now = millis();
    if (now - last_update > 100) {
      display.display1(++value);
      display.display2(shift_time);
      last_update = now;
      iters = 0;
      shift_time = 0;
    }
    display.refresh();
    delayMicroseconds(5000);
    iters++;
  }
}
