/* blink.c
 * 
 * A simple LED-blinking program which does NOT use the Arduino libraries
 * but directly uses the AVR registers, and interrupts.  
 * 
 * This blink program is designed to run on the ATmega644 with a 20Mhz Clock
 *
 * This example code is in the public domain.
 */

#include <avr/io.h>
#include <avr/interrupt.h>


#define _NOP() do { __asm__ volatile ("nop"); } while (0)

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

void yield();

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;

  // Toggle PIN PB4 on every overflow
	PORTB ^= _BV(PB4);
}

unsigned long micros() {
	unsigned long m;
	uint8_t oldSREG = SREG, t;
	
	cli();
	m = timer0_overflow_count;
	t = TCNT0;

	if ((TIFR0 & _BV(TOV0)) && (t < 255))
		m++;

	SREG = oldSREG;
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void delay(unsigned long ms) {
	uint16_t start = (uint16_t)micros();

	while (ms > 0) {
		if (((uint16_t)micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
}

/* perform one-time initialization */
void init() {
	// set timer 0 prescale factor to 64
	// this combination is for the standard 168/328/1280/2560
	TCCR0B = 0x03;

	// enable timer 0 overflow interrupt
	sbi(TIMSK0, TOIE0);

	// the bootloader connects pins 0 and 1 to the USART; disconnect them
	// here so they can be used as normal digital i/o; they will be
	// reconnected in Serial.begin()
	UCSR0B = 0;

	TCNT0 = 0;
	OCR0A = 0;
	OCR0B = 0;

	sei();
}

int main() {
	init();

	// Set direction of PIN D6 to be output
	DDRD = (1<<DDD6);
	PORTD = 0;

	// Add nop for synchronization.... why???
	_NOP();

	for (;;) {
		PIND |= (1<<PD6);    // Turn pin D6 on
		delay(500);          // Delay 500 miliseconds
		PIND &= ~(1<<PD6);   // Turn pin D6 off
		delay(500);          // Delay 500 miliseconds
	}

	return 0;
}
