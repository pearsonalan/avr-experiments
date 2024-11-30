#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Arduino.h>

// Class to manage a pair of 6-digit seven segment display
//
// Character control signals for each of the 6 digits are attached to
// PC0..PC5.  Control signals for the seven segments are attached to
// the output pins of a shift register.
class Display {
 public:
  Display(bool leading_zeros, int data_pin, int clock_pin, int latch_pin);

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

  // Pin connected to SER of 74HC595
  const int data_pin_;

  // Pin connected to SRCLK (Shift Register Clock) of 74HC595
  const int clock_pin_;

  // Pin connected to RCLK (Storage Register Clock or latch pin) of 74HC595
  const int latch_pin_;
};

#endif