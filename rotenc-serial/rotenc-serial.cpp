#include <Arduino.h>

volatile int val = 180;

// Initialize the inputs for signal A and signal B from the rotary encoder.
// They are hooked up to PCINT1 on PB1 and GPIO pin PB2. Because the software is
// not performing debouncing, pull-ups and external debounce hardware is
// required.
void rotary_spin_int_init(void) {
  // Set PB1 and PB2 pin as inputs (Clear bits corresponding to PB1, PB2)
  DDRB &= ~_BV(PB1) & ~_BV(PB2);
  // Enable pin change interrupt pin PCINT1
  PCMSK0 |= (1 << PCINT1);
  // Enable pin change interrupt PCIE0 for bank PCINT[7:0]
  PCICR |= (1 << PCIE0);
}

// Initialize inputs for signal C from the rotary encoder (button). This
// is hooked up to PCINT0 on PB0. An external pull-up resistor and debounce
// hardware is required.
void rotary_button_int_init(void) {
  DDRB &= ~_BV(PB0);
  // Enable pin change interrupt pin PCINT0
  PCMSK0 |= (1 << PCINT0);
  // Enable pin change interrupt PCIE0 for bank PCINT[7:0]
  PCICR |= (1 << PCIE0);
}

int turn_history[4] = {0};
volatile bool rotenc_changed = false;

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
  uint8_t a = (PINB & _BV(PB2)) == _BV(PB2);
  uint8_t b = (PINB & _BV(PB1)) == _BV(PB1);
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
  rotenc_changed = true;

  turn_history[0]++;
}

void button_press() {
  val = 180;
  rotenc_changed = true;
}

volatile uint8_t pb0_last_state;
volatile uint8_t pb1_last_state;

ISR(PCINT0_vect) {
  uint8_t pb0_state = (PINB & _BV(PB0)) == _BV(PB0);
  uint8_t pb1_state = (PINB & _BV(PB1)) == _BV(PB1);

  if (pb1_state != pb1_last_state) {
    rotenc_turn();
  }

  if (pb0_state != pb0_last_state) {
    button_press();
  }

  pb0_last_state = pb0_state;
  pb1_last_state = pb1_state;
}

void setup() {
  cli();
  rotary_spin_int_init();
  rotary_button_int_init();
  sei();

  Serial.begin(115200);
  while (!Serial) {}

  Serial.print("VAL: ");
  Serial.println(val);

  pb0_last_state = (PINB & _BV(PB0)) == _BV(PB0);
  pb1_last_state = (PINB & _BV(PB1)) == _BV(PB1);
}

void loop() {
  static int n = 0;
  static uint8_t button_state = 0;
  delay(2);

  if (++n == 150) {
    turn_history[3] = turn_history[2];
    turn_history[2] = turn_history[1];
    turn_history[1] = turn_history[0];
    turn_history[0] = 0;
    n = 0;
  }

  uint8_t button = (PINB & _BV(PB0)) == _BV(PB0);
  if (button != button_state) {
    Serial.print("BUTTON: ");
    Serial.println(button);
    button_state = button;
  }

  if (rotenc_changed) {
    rotenc_changed = false;
    Serial.print("VAL: ");
    Serial.println(val);
  }
}
