/* #include "i2c.h" */
#include <avr/io.h>
#include <util/delay.h>

void initTimer(void)
{
	// Timer/Counter Control Register 0A
	TCCR1A = _BV(WGM01); // Set up 16 bit clock (TCCR0A) in CTC mode
	
	// Timer/Counter Countrol Register 0B
	TCCR1B = _BV(CS10) | _BV(CS12); // Clock prescaler set at 1/1024

	// Timer Interrupt Mask Register
	TIMSK1 = _BV(OCIE1A); // Enable interrupts on (Output Compare Interrupt Enable) 1A
	// 1 comes from the timer number, A comes from the timer vector used in the ISR
	
	// Output Compare Register
	OCR1A = 15624; // Timer will reset every time it hits 15624 (100 kHz) 
}

ISR(TIMER1_COMPA_vect)
{
	PORTB ^= _BV(PB0);
}

int main (void)
{
	DDRB |= _BV(PB0);

	SEI(); // Enable interrupts globally
	initTimer(); // Initialize the timer

}
