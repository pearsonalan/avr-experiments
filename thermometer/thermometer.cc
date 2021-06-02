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

  void SetLeadingZeros(bool value) {
    leading_zeros_ = value;
  }

  // Set the value (0-9999) to show on the 4-character display.  If dp is not
  // -1, the corresponding decimal point is also turned on.
  void DisplayNumber(unsigned long value, int dp = -1);

  // Set the pattern to show on the 4-character display
  void DisplayPattern(const uint8_t *pattern);

  // Must be called approximately every 2ms to keep the Persistence-of-vision
  // effect showing all 4 characters.
  void Refresh();

 private:
  uint8_t active_character_ = 0;
  bool leading_zeros_ = false;

  uint8_t pattern_[4] = {0};

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

void Display::DisplayNumber(unsigned long value, int dp) {
  for (uint8_t i = 0; i < 4; i++) {
    bool show_char = (value != 0 || leading_zeros_);
    bool show_dp = dp == 3 - i;
    uint8_t digit = (uint8_t)(value % 10);

    uint8_t pattern = 0;
    if (show_char) pattern = patterns[digit];
    if (show_dp) pattern |= 0x80;
    pattern_[3 - i] = pattern;

    value = value / 10;
  }
}

void Display::DisplayPattern(const uint8_t *pattern) {
  pattern_[0] = pattern[0];
  pattern_[1] = pattern[1];
  pattern_[2] = pattern[2];
  pattern_[3] = pattern[3];
}

void Display::Refresh() {
  int prev_active_character = active_character_;
  // advance to the next character
  if (++active_character_ == 4) active_character_ = 0;

  // load the pattern to show for the active character
  int pattern = pattern_[active_character_];

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
// Interrupt handlers to handle input buttons
//

constexpr int8_t DisplayCelsius = 0;
constexpr int8_t DisplayFarenheight = 1;
constexpr int8_t DisplayADC = 2;
constexpr int8_t DisplayVoltage = 3;
constexpr int8_t DisplayResistance = 4;

// Button attached to INT0 will cycle through the display modes
// setting mode_changed = 1 on each press.

volatile int8_t mode = DisplayCelsius;
volatile int8_t mode_changed = 1;

ISR(INT0_vect) {
  if (mode == DisplayResistance) {
    mode = DisplayCelsius;
  } else {
    ++mode;
  }
  mode_changed = 1;
}


// Button attached to INT1 will cycle through history
volatile int8_t history_button_press = 0;

ISR(INT1_vect) {
  history_button_press = 1;
  mode_changed = 1;
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

// Class to periodically sample an ADC pin and average the value in a
// ring buffer for smoothing.
class Sampler {
public:
  Sampler(int pin);

  // Calculate and return the average of the samples
  int CalculateAverage() const;

  // Call frequently to have the sampler check if a new sample is due
  // to be take. Returns 1 if a sample was read, else 0
  uint8_t MaybeSample(unsigned long now);

private:
  // Number of samples to retain in the ring buffer
  static constexpr int SAMPLE_SIZE = 8;

  // Number of milliseconds between reading the ADC
  static constexpr int SAMPLE_INTERVAL = 1000;

  // Analog pin to sample
  const int pin_;

  // Keep a ring buffer of samples from the ADC of the thermistor
  // to filter out noise.
  int samples_[SAMPLE_SIZE] = {0};
  int sample_pos_ = 0;

  // Time in milliseconds the last sample was taken
  unsigned long last_sample_time_ = 0;
};


Sampler::Sampler(int pin) : pin_(pin) {
  // Populate the ring buffer initially with readings from the ADC
  // so we don't have to worry about averaging over a partially-filled
  // ring buffer.
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    samples_[i] = analogRead(pin_);
  }
}

uint8_t Sampler::MaybeSample(unsigned long now) {
  if (now - last_sample_time_ > SAMPLE_INTERVAL) {
    // Insert a sample into the ring buffer and move to the 
    // next postion, or wrap around
    samples_[sample_pos_] = analogRead(pin_);
    if (++sample_pos_ == SAMPLE_SIZE) sample_pos_ = 0;
    last_sample_time_ = now;
    return 1;
  }
  return 0;
}

int Sampler::CalculateAverage() const {
  int sum = 0;
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    sum += samples_[i];
  }
  return sum / 8;
}

// Class to keep a history of samples over a longer duration than the
// ring buffer of the sampler.  This is used for "history browsing" of
// previous samples
class History {
public:
  History() {}

  // Update the history with the given sample if the time for an update
  // has been reached
  void MaybeUpdate(int sample, unsigned long now);

  int sample_count() const { return sample_count_; }

  // Gets the n-th prior sample. Get(1) returns the prior entry,
  // Get(2) returns the entry before that, etc. Values of n greater than
  // HISTORY_SIZE will "wrap around".
  int Get(int n);

private:
  // The number of history items to retain
  static constexpr int HISTORY_SIZE = 24;

  // Number of milliseconds between reading the ADC
  static constexpr unsigned long SAMPLE_INTERVAL = 3600UL * 1000UL;

  int history_[HISTORY_SIZE] = {0};
  int sample_pos_ = 0;
  int sample_count_ = 0;

  // Time in milliseconds the last sample was taken
  unsigned long last_sample_time_ = 0;
};

void History::MaybeUpdate(int sample, unsigned long now) {
  if (now - last_sample_time_ > SAMPLE_INTERVAL) {
    // Insert a sample into the history and move to the 
    // next postion, or wrap around
    history_[sample_pos_] = sample;
    if (sample_count_ < HISTORY_SIZE) ++sample_count_;
    if (++sample_pos_ == HISTORY_SIZE) sample_pos_ = 0;
    last_sample_time_ = now;
  }
}

int History::Get(int n) {
  return history_[((sample_pos_ - n) + HISTORY_SIZE) % HISTORY_SIZE];
}


// This class controls the display of history on the seven-segment display.
class HistoryBrowser {
 public:
  HistoryBrowser(const History& history) : history_(history) {}

  // Call periodically to refresh the history browser display based on the
  // presses of the history button and the passage of time. Returns true
  // if the display needs to be updated.
  bool Refresh(unsigned long now);
  
  // Returns true if the display should show the history label.
  bool IsShowingLabel() const {
    return showing_label_ == 1;
  }

  // Returns true if the display should show a history value.
  bool IsShowingValue() const {
    return history_item_ != 0 && showing_label_ == 0;
  }

  void ShowLabel(Display* display) const;

  // Return the index of the history item to show.
  int8_t history_item() const {
    return history_item_;
  }

 private:
  const History& history_;

  // Which item in the history to show.
  int8_t history_item_ = 0;

  // When set to 1, display the label of the history item rather than the
  // value.
  bool showing_label_ = false;

  unsigned long last_update_time_ = 0;
};

void HistoryBrowser::ShowLabel(Display* display) const {
  uint8_t pattern[4] = {0};
  if (history_.sample_count() == 0) {
    // 'n' => Segments C,E,G  =>    0101 0100  =>  0x54
    pattern[0] = 0x54;
    // 'o' => Segments C,D,E,G  =>  0101 1100  =>  0x5C
    pattern[1] = 0x5C;
    // 'h' => Segments C,E,F,G  =>  0111 0100  =>  0x74
    pattern[3] = 0x74;
  } else {
    if (history_item_ >= 10) {
      pattern[0] = patterns[(history_item_ / 10) % 10];
    }
    pattern[1] = patterns[history_item_ % 10];
    // 'h' => Segments C,E,F,G  =>  0111 0100  =>  0x74
    pattern[3] = 0x74;
  }

  display->DisplayPattern(pattern);
}

bool HistoryBrowser::Refresh(const unsigned long now) {
  bool display_changed = false;
  // Determine if the History button has been pressed since last call to
  // Refresh.
  if (history_button_press == 1) {
    // Button is pressed. Increment the history item being viewed and
    // initially show the label. 
    display_changed = true;
    history_item_++;
    if (history_item_ > history_.sample_count()) {
      // Wrap around but to 1 instead of 0 since we want to stay in the history
      // list
      history_item_ = 1;
    }
    showing_label_ = true;
    last_update_time_ = now;
    history_button_press = 0;
  } else if (history_item_ != 0) {
    if (now - last_update_time_ > 500 && showing_label_) {
      // If showing history label and 500ms has elapsed, turn off the label.
      display_changed = true;
      showing_label_ = false;
      // If there is no history, go back go showing live data
      if (history_.sample_count() == 0) history_item_ = 0;
    }
    if (now - last_update_time_ > 5000) {
      // If showing history value and 5s has elapsed, turn off the history and
      // go back to showing live data.
      display_changed = true;
      history_item_ = 0;
    }
  }
  return display_changed;
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
  
  // Set INT0 and INT1 to trigger on the falling edge (button press)
  EICRA = 0x0A;

  // Enable INT0 and INT1
  EIMSK = 0x03;

  static Display display(false, DATA_PIN, CLOCK_PIN, LATCH_PIN,
                  {CC0, CC1, CC2, CC3});

  // Keep a ring buffer of samples from the ADC of the thermistor
  // to filter out noise.
  static Sampler sampler(THERMISTOR_PIN);

  // Keeps a history of samples
  static History history;

  // Controls display while browsing history.
  static HistoryBrowser history_browser(history);

  for (;;) {
    unsigned long now = millis();
    bool display_changed = mode_changed == 1;
    display_changed |= sampler.MaybeSample(now);
    display_changed |= history_browser.Refresh(now);

    // Update the value shown on the display if the display mode has changed
    // or if there is a new reading.
    if (display_changed) {
      if (history_browser.IsShowingLabel()) {
        history_browser.ShowLabel(&display);
      } else {
        // Get the average of the samples from the ring buffer.
        int average_sample = sampler.CalculateAverage();

        // Allow the history to be updated with the new sample if the time is
        // due
        history.MaybeUpdate(average_sample, now);

        // Determine which sample to show on the display. This could be a
        // reading from the history or the last live sample
        int display_sample = average_sample;
        if (history_browser.IsShowingValue()) {
          display_sample = history.Get(history_browser.history_item());
        }

        // Calculate the voltage and temperature from the sample.
        int thermistor_voltage_mv =
            (int)((long)display_sample * 5L * 1000L / 1023L);
        int thermistor_resistance_ohms =
            (int)(SERIES_RESISTOR / (1023.0 / (float)display_sample - 1.0));
        float temperature_celsius =
            ResistanceToCelsius(thermistor_resistance_ohms);

        // Set up the display to show a value based on which mode the device
        //  is in
        switch (mode) {
        case DisplayCelsius:
          // Display temperature in degrees Celsius
          display.DisplayNumber((int)(temperature_celsius * 10), 2);
          break;

        case DisplayFarenheight:
          // Display temperature in degrees Farenheight
          display.DisplayNumber(
              (int)(CelsiusToFarenheight(temperature_celsius) * 10), 2);
          break;

        case DisplayADC:
          // Display the raw sample data
          display.DisplayNumber(display_sample);
          break;

        case DisplayVoltage:
          // Display the calculated voltage drop across the thermistor.
          display.DisplayNumber(thermistor_voltage_mv, 0);
          break;

        case DisplayResistance:
          // Display the calculated resistance of the thermistor by the voltage
          // divider formula in KOhms.
          if (thermistor_resistance_ohms > 10000) {
            display.DisplayNumber(thermistor_resistance_ohms / 10, 1);
          } else {
            display.DisplayNumber(thermistor_resistance_ohms, 0);
          }
          break;
        }
      }
      mode_changed = 0;
    }

    display.Refresh();
    delay(2);
  }
}
