//
// This file is based on the atmega32_rotenc_test.c from simavr.
// 
// The simavr copyright is reproduced below.
//
// I have adapted the original program to use the ATmega328 rather
// than the ATmega32 CPU. I have also reformatted the code to suit
// my style preferences :-)
//
// For the ATmega328, we will use PORTD[0:7] as the LED outputs. The rotary 
// encoder has 3 signals A & B which indicate the turning direction, and C
// which is the button press. Signal A is be wired to PB2 and will raise
// PCINT2 (Interrupt Vector PCI0). Signal B is wired to PB1. The code does not
// require a pin change interrupt on signal B.  Signal C is wired to PC0 and
// will raise PCINT8 (Interrupt Vector PCI1).
//

/*
 atmega32_rotenc_test.c

 A simple example demonstrating a Pansonic EVEP rotary encoder
 scrolling an LED up and down an 8 segment LED bar.

 Copyright 2018 Doug Szumski <d.s.szumski@gmail.com>

 This file is part of simavr.

 simavr is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 simavr is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */

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
  // Set PB2 and PB1 pin as inputs (Clear bits corresponding to PB2, PB1)
  DDRB &= ~(1 << PB2) & ~(1 << PB1);
  // Enable pin change interrupt pin PCINT2
  PCMSK0 |= (1 << PCINT2);
  // Enable pin change interrupt PCIE0 for bank PCINT[7:0]
  PCICR |= (1 << PCIE0);
}

/* Initialize inputs for signal C from the rotary encoder (button). This
 * is hooked up to PCINT8 on PC0. An external pull-up resistor and debounce
 * hardware is required. */
void rotary_button_int_init(void) {
  DDRC &= ~(1 << PC0);
  // Enable pin change interrupt pin PCINT8
  PCMSK1 |= (1 << PCINT8);
  // Enable pin change interrupt PCIE0 for bank PCINT[14:8]
  PCICR |= (1 << PCIE1);
}

/* Configure 8 segment virtual 'LED bar' on port D */
void led_bar_init(void) {
  // Since PD0 and PD1 are the RXD and TXD pins, we have to disable the USART
  UCSR0B &= ~(1<<RXEN0) & ~(1<<TXEN0);

  TCCR0A = 0;
  EIMSK = 0;
  EICRA = 0;

  DDRD = 0xFF; // All outputs
  PORTD = 0xFF; // Start with all lights on... a button press is required 
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
  uint8_t a = (PINB & _BV(PB2)) == _BV(PB2);
  uint8_t b = (PINB & _BV(PB1)) == _BV(PB1);
  uint8_t ccw_twist = a ^ b;

  if (!a) 
    return;

  // Scroll the LED bar
  if (ccw_twist) {
    led_bar_scroll(COUNTERCLOCKWISE);
  } else {
    led_bar_scroll(CLOCKWISE);
  }
}

ISR(PCINT1_vect) {
  // Fires when rotary encoder button was pressed and resets
  // the LED bar to the starting position
  PORTD = 0b00010000;
}
