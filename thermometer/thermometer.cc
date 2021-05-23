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

// Class to manage a 4-digit seven segment display attached to a shift
// register.
//
// Character control signals for each of the 4 digits are attached to
// char_control pinds.  Control signals for the seven segments are attached to
// the output pins of a shift register.
class Display {
 public:
  Display(bool leading_zeros, int data_pin, int clock_pin, int latch_pin,
          const int (&char_control_pins)[4]);

  // Set the value (0-9999) to show on the 4-character display
  void display(unsigned long value);

  // Enables display of the decimal point at the given position
  void enableDP(int p) { show_dp_[p] = true; }

  // Turns off the decimal point at all positions
  void disableDP() { 
    for (int p = 0; p < 4; p++) {
      show_dp_[p] = false;
    }
  }

  // Must be called approximately every 2ms to keep the Persistence-of-vision
  // effect showing all 4 characters.
  void refresh();

 private:
  uint8_t active_character_ = 0;
  bool leading_zeros_ = false;

  uint8_t digits_[4] = {0};
  bool show_char_[4] = {false};
  bool show_dp_[4] = {false};

  // Pin connected to SER of 74HC595
  const int data_pin_;

  // Pin connected to SRCLK (Shift Register Clock) of 74HC595
  const int clock_pin_;

  // Pin connected to RCLK (Storage Register Clock or latch pin) of 74HC595
  const int latch_pin_;

  // Control pins to enable the 4 characters of the seven segment display
  int char_control_pins_[4];
};


Display::Display(bool leading_zeros, int data_pin,
                 int clock_pin, int latch_pin,
                 const int (&char_control_pins)[4])
    : leading_zeros_(leading_zeros),
      data_pin_(data_pin),
      clock_pin_(clock_pin),
      latch_pin_(latch_pin),
      char_control_pins_{char_control_pins[0], char_control_pins[1],
                         char_control_pins[2], char_control_pins[3]}
{
  // Set pins attached to the shift register to output mode
  pinMode(latch_pin_, OUTPUT);
  pinMode(clock_pin_, OUTPUT);
  pinMode(data_pin_, OUTPUT);

  // Set pins attached to 7-seg-display common cathodes to output.
  // Set HIGH to initially disable all characters. The pins will be
  // set LOW when the shift register has been loaded with a value to
  // display.
  for (int i = 0; i < 4; i++) {
    pinMode(char_control_pins_[i], OUTPUT);
    digitalWrite(char_control_pins_[i], HIGH);
  }
}

void Display::display(unsigned long value) {
  for (uint8_t i = 0; i < 4; i++) {
    show_char_[3 - i] = (value != 0 || leading_zeros_);
    digits_[3 - i] = (uint8_t)(value % 10);
    value = value / 10;
  }
}

void Display::refresh() {
  int prev_active_character = active_character_;
  // advance to the next character
  if (++active_character_ == 4) active_character_ = 0;

  // load the patterns to show for the active character
  int pattern = patterns[digits_[active_character_]];
  if (!show_char_[active_character_]) pattern = 0;
  if (show_dp_[active_character_]) pattern |= 0x80;

  // Set the latchPin low so the LEDs don't change while sending in bits
  digitalWrite(latch_pin_, LOW);

  // Shift out the bits
  shiftOut(data_pin_, clock_pin_, MSBFIRST, pattern);

  // Turn off the character that was active (set enable pin HIGH)
  digitalWrite(char_control_pins_[prev_active_character], HIGH);

  // Set the latch pin high so the shift register output pins will 
  // output the pattern loaded into the register.
  digitalWrite(latch_pin_, HIGH);

  // Drive the character enable pin low to turn on the character
  digitalWrite(char_control_pins_[active_character_], LOW);
}


//
// Controller class to manage buttons
// This class uses INT0 and INT1 to read the state of buttons attached
// to PD2 and PD3.
//

constexpr int8_t DisplayCelsius = 0;
constexpr int8_t DisplayFarenheight = 1;
constexpr int8_t DisplayADC = 2;
constexpr int8_t DisplayVoltage = 3;
constexpr int8_t DisplayResistance = 4;

volatile int8_t mode = DisplayCelsius;
volatile int8_t mode_changed = 1;

ISR(INT0_vect) {
  static int8_t prev_state = HIGH;
  int8_t state = digitalRead(2);
  if (state == LOW && prev_state == HIGH) {
    if (mode == DisplayResistance) {
      mode = DisplayCelsius;
    } else {
      ++mode;
    }
    mode_changed = 1;
  }
  prev_state = state;
}

ISR(INT1_vect) {
}

//
// PIN Definitions
//

// Pin connected to RCLK (Storage Register Clock or latch pin) of 74HC595
constexpr int LATCH_PIN = 10;

// Pin connected to SRCLK (Shift Register Clock) of 74HC595
constexpr int CLOCK_PIN = 9;

// Pin connected to SER of 74HC595
constexpr int DATA_PIN = 8;

constexpr int CC0 = A3;
constexpr int CC1 = A2;
constexpr int CC2 = A1;
constexpr int CC3 = A0;

constexpr int THERMISTOR_PIN = A4;


// Number of milliseconds between reading the ADC
constexpr int SAMPLE_INTERVAL = 1000;

int CalculateAverage(const int (&samples)[8]) {
  int sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += samples[i];
  }
  return sum / 8;
}

// Resistance of the resistor in series with the thermistor.
// Can assume a 10K resistor or take an actual measurement.
constexpr float SERIES_RESISTOR = 10000.0;

// Resistance of the thermistor at the temperature TEMPERATURENORMAL
constexpr float THERMISTORNORMAL = 10000.0;
constexpr float BCOEFFICIENT = 3950.0;
constexpr float TEMPERATURENORMAL = 25.0;

// Compute the temperature in celsius for a given resistance of
// the thermistor whose parameters are defined by the contants
// THERMISTORNORMAL, TEMPERATURENORMAL and BCOEFFICIENT.
float ResistanceToCelsius(float r) {
  return 1.0 / (log(r / THERMISTORNORMAL) / BCOEFFICIENT +
                1.0 / (TEMPERATURENORMAL + 273.15)) - 273.15;
}

float CelsiusToFarenheight(float c) {
  return (c * 9.0 / 5.0) + 32.0;
}


//
// MAIN entry point
//

int main() {
  init();
  
  // Set INT0 and INT1 to trigger on any logic change
  EICRA = 0x05;

  // Enable INT0 and INT1
  EIMSK = 0x03;

  Display display(false, DATA_PIN, CLOCK_PIN, LATCH_PIN,
                  {CC0, CC1, CC2, CC3});

  // Keep a ring buffer of 8 samples from the ADC of the thermistor
  // to filter out noise.
  int samples[8];
  int sample_pos = 0;

  // Populate the ring buffer initially with 8 readings from the ADC
  // so we don't have to worry about a partially-filled ring buffer
  for (int i = 0; i < 8; i++) {
    samples[i] = analogRead(THERMISTOR_PIN);
  }

  unsigned long last_reading_time = 0;
  int8_t reading_changed = 0;

  for (;;) {
    // Read a sample periodically
    unsigned long now = millis();
    if (now - last_reading_time > SAMPLE_INTERVAL) {
      // Insert a sample into the ring buffer and move to the 
      // next postion, or wrap around
      samples[sample_pos] = analogRead(THERMISTOR_PIN);
      if (++sample_pos == 8) sample_pos = 0;
      reading_changed = 1;
      last_reading_time = now;
    }

    // Update the value shown on the display if the display mode has changed
    // or if there is a new reading.
    if (mode_changed || reading_changed) {
      // Get the average of the samples from the ring buffer and calculate
      // the temperature from the sample.
      int average_sample = CalculateAverage(samples);
      int thermistor_voltage_mv =
          (int)((long)average_sample * 5L * 1000L / 1023L);
      int thermistor_resistance_ohms =
          (int)(SERIES_RESISTOR / (1023.0 / (float)average_sample - 1.0));
      float temperature_celsius =
          ResistanceToCelsius(thermistor_resistance_ohms);

      switch (mode) {
        case DisplayCelsius:
          display.display((int)(temperature_celsius * 10));
          display.disableDP();
          display.enableDP(2);
          break;
        case DisplayFarenheight:
          display.display((int)(CelsiusToFarenheight(temperature_celsius) * 10));
          display.disableDP();
          display.enableDP(2);
          break;
        case DisplayADC:
          display.display(average_sample);
          display.disableDP();
          break;
        case DisplayVoltage:
          display.display(thermistor_voltage_mv);
          display.enableDP(0);
          break;
        case DisplayResistance:
          display.display(thermistor_resistance_ohms);
          display.disableDP();
          break;
      }

      mode_changed = 0;
      reading_changed = 0;
    }

    display.refresh();
    delay(2);
  }
}
