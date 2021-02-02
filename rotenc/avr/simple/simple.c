#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>

typedef enum {
  CLOCKWISE,
  COUNTERCLOCKWISE
} led_scroll_dir_t;

/* Initialize the inputs for signal A and signal B from the rotary encoder.
 * They are hooked up to PCINTO on PB0 and GPIO pin PB1. Because the software is
 * not performing debouncing, pull-ups and external debounce hardware is
 * required. */
void rotary_spin_int_init(void) {
#if 0
  // Set PB0 and PB1 pin as inputs (Clear bits corresponding to PB0, PB1)
  DDRB &= ~(1 << PB0) & ~(1 << PB1);
  // Enable pin change interrupt pin PCINT0
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0);
#endif
  DDRB = 0b11111001;
  PCICR = (1 << PCIE0) | (1 << PCIE1);
  PCMSK0 = (1 << PCINT2);
}


/* Initialize inputs for signal C from the rotary encoder (button). This
 * is hooked up to PCINT8 on PC0. An external pull-up resistor and debounce
 * hardware is required. */
void rotary_button_int_init(void) {
#if 0
  DDRC &= ~(1 << PC0);
  // Enable pin change interrupt pin PCINT8
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT8);
#endif
  DDRC = 0b11111110;
  PCICR = (1 << PCIE0) | (1 << PCIE1);
  PCMSK1 = (1 << PCINT8);
}

/* Configure 8 segment virtual 'LED bar' on port D */
void led_bar_init(void) {
  // Since PD0 and PD1 are the RXD and TXD pins, we have to disable the USART
  UCSR0B &= ~(1<<RXEN0) & ~(1<<TXEN0);

  DDRD = 0xFF; // All outputs
  PORTD = 0b00010000; // Start with a light on in the middle
}

void led_bar_scroll(led_scroll_dir_t dir) {
  switch (dir) {
    case CLOCKWISE:
      if (PORTD < 0b10000000) PORTD <<= 1;
      break;
    case COUNTERCLOCKWISE:
      if (PORTD > 0b00000001) PORTD >>= 1;
      break;
    default:
      break;
  }
}


int main() {
  // Disable interrupts whilst configuring them to avoid false triggers
  cli();
  rotary_spin_int_init();
  rotary_button_int_init();
  sei();

  led_bar_init();

  while (1) {
    // Wait for some input
  }

  cli();
  sleep_mode();
}

ISR(PCINT0_vect) {
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
#if 0
  uint8_t ccw_twist = (PINB & (1 << PB0)) ^ ((PINB & (1 << PB1)) >> 1);

  // Scroll the LED bar
  if (ccw_twist) {
    led_bar_scroll(COUNTERCLOCKWISE);
  } else {
    led_bar_scroll(CLOCKWISE);
  }
#endif
  
  PORTD = 0x80;
}

ISR(PCINT1_vect) {
  // Fires when rotary encoder button was pressed and resets
  // the LED bar to the starting position
  PORTD = 0x01;
}
