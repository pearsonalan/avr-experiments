#include <avr/io.h>
#include <assert.h>

#define nop() asm("nop")

#define BUFSZ 32

char a[BUFSZ];
char b[BUFSZ];

void avr_memcpy(char *s, char *d, int l)
{
	int i;

	for (i = 0; i < l; i++)
		*d++ = *s++;
}

void toggle_b0()
{
	/* toggle pin B0 on and off */
	PORTB = (1<<PINB0);
	nop();
	PORTB = (0<<PINB0);
	nop();
}


int main()
{
	int i;

	/* configure pin 0 of port B to be an output pin */
	DDRB = (1<<DDB0);
	PORTB = (0<<PINB0);
	nop();

	toggle_b0();

	/* write some values to buffer a */
	for (i = 0; i < BUFSZ; i++)
		a[i] = (char)i;

	toggle_b0();

	/* copy buffer a to buffer b */
	avr_memcpy(a, b, BUFSZ);

	toggle_b0();

	/* check that they are the same */
	for (i = 0; i < BUFSZ; i++)
		assert(a[i] == b[i]);

	
	return 0;
}
