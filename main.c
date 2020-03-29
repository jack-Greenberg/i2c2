#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

int main (void)
{
	DDRB |= _BV(PB0);

	SEI(); // Enable interrupts globally
	initTimer(); // Initialize the timer

}
