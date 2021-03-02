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

constexpr int char_pins[4] = {
  PB1, PB2, PB3, PB4
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
  // Set the bits in DDRB to indicate PB1..PB4 are output
  DDRB |= _BV(PB1) | _BV(PB2) | _BV(PB3) | _BV(PB4);

  // Set all characters off.  We need to drive the character enable pin low
  // to allow current to flow through the LEDs
  PORTB |= _BV(PB1) | _BV(PB2) | _BV(PB3) | _BV(PB4);

  // Set all pins in PORTD to be output
  DDRD = 0xFF;

  // Turn off all segments initially
  PORTD = 0;
}

void Display::display(int value) {
  for (int i = 0; i < 4; i++) {
    show_char_[3 - i] = (value != 0 || leading_zeros_);
    digits_[3 - i] = value % 10;
    value = value / 10;
  }
}

void Display::refresh() {
  // Turn off the character that was active (set enable pin HIGH)
  PORTB |= _BV(char_pins[active_character_]);

  // advance to the next character
  if (++active_character_ == 4) active_character_ = 0;

  // load the pattern to show for the given character
  int pattern = patterns[digits_[active_character_]];
  if (!show_char_[active_character_]) pattern = 0;

  // set the pattern
  PORTD = pattern;

  // Drive the character enable pin low to turn on the character
  PORTB &= ~_BV(char_pins[active_character_]);
}

Display display(true);

static int val = 180;

// Initialize the inputs for signal A and signal B from the rotary encoder.
// They are hooked up to PCINT8 on PC0 and GPIO pin PC1. Because the software is
// not performing debouncing, pull-ups and external debounce hardware is
// required.
void rotary_spin_int_init(void) {
  // Set PC0 and PC1 pin as inputs (Clear bits corresponding to PC0, PC1)
  DDRC &= ~_BV(PC0) & ~_BV(PC1);
  // Enable pin change interrupt pin PCINT8
  PCMSK1 |= (1 << PCINT8);
  // Enable pin change interrupt PCIE1 for bank PCINT[14:8]
  PCICR |= (1 << PCIE1);
}

// Initialize inputs for signal C from the rotary encoder (button). This
// is hooked up to PCINT10 on PC2. An external pull-up resistor and debounce
// hardware is required.
void rotary_button_int_init(void) {
  DDRC &= ~_BV(PC2);
  // Enable pin change interrupt pin PCINT10
  PCMSK1 |= (1 << PCINT10);
  // Enable pin change interrupt PCIE1 for bank PCINT[14:8]
  PCICR |= (1 << PCIE1);
}

int turn_history[4] = {0};

void rotenc_turn() {
  // The Panasonic EVEP rotary encoder this is written for moves two
  // phases for every 'click' it emits. The interrupt is configured
  // to fire on every edge change of B, which is once per 'click'.
  // Moving forwards in phase, after B has changed state we poll A.
  // We get the sequence (A=0,B=1), (A=1,B=0). Moving backwards in
  // phase, after B has changed state we get (A=1,B=1), (A=0,B=0).
  //
  // +-------+---+---+
  // | Phase | A | B |
  // +-------+---+---+
  // |   0   | 0 | 0 |
  // |   1   | 0 | 1 |
  // |   2   | 1 | 1 |
  // |   3   | 1 | 0 |
  // +-------+---+---+
  //
  // The twist direction is then obtained by taking the logical
  // XOR of outputs A and B after the interrupt has fired. Of
  // course with a 'real life' part you might be better off
  // using a state machine, or some extra hardware to filter out
  // contact bounces which aren't modelled in the virtual part.
  uint8_t a = (PINC & _BV(PC1)) == _BV(PC1);
  uint8_t b = (PINC & _BV(PC0)) == _BV(PC0);
  uint8_t ccw_twist = a ^ b;

  if (!a) 
    return;

  int inc = 1;
  int t = 5; /// threshold for fast turns
  if (turn_history[1] > t || turn_history[2] > t || turn_history[3] > t)
    inc = 5;

  // Scroll the LED bar
  if (ccw_twist) {
    val -= inc; 
    if (val < 0) val += 360;
  } else {
    val += inc;
    if (val >= 360) val -= 360;
  }

  turn_history[0]++;
}

void button_press() {
  val = 180;
}

uint8_t pc0_last_state;
uint8_t pc2_last_state;

ISR(PCINT1_vect) {
  uint8_t pc0_state = (PINC & _BV(PC0)) == _BV(PC0);
  uint8_t pc2_state = (PINC & _BV(PC2)) == _BV(PC2);

  if (pc0_state != pc0_last_state) {
    rotenc_turn();
  }

  if (pc2_state != pc2_last_state) {
    button_press();
  }

  pc0_last_state = pc0_state;
  pc2_last_state = pc2_state;

  display.display(val);
}

void setup() {
  // Since PD0 and PD1 are the RXD and TXD pins, we have to disable the USART
  UCSR0B &= ~(1<<RXEN0) & ~(1<<TXEN0);

  TCCR0A = 0;
  EIMSK = 0;
  EICRA = 0;

  display.display(val);

  cli();
  rotary_spin_int_init();
  rotary_button_int_init();
  sei();

  pc0_last_state = (PINC & _BV(PC0)) == _BV(PC0);
  pc2_last_state = (PINC & _BV(PC2)) == _BV(PC2);
}

void loop() {
  static int n = 0;
  display.refresh(); 
  delay(2);

  if (++n == 150) {
    turn_history[3] = turn_history[2];
    turn_history[2] = turn_history[1];
    turn_history[1] = turn_history[0];
    turn_history[0] = 0;
    n = 0;
  }
}
