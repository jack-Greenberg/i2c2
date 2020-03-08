#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

#define BIT_MASK 0x1;

volatile uint32_t globalTimerFlag = 0x01;
uint8_t gStartTimerFlag = 0;
uint8_t control = 0x01;

void init_I2C(int baud_rate)
{
	// Stop interrupts and reset Timer 1
	cli();
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	TCCR1B |= _BV(WGM12); // CTC mode
	TCCR1B |= _BV(CS10); // PRESCALER 1
	TIMSK1 |= _BV(OCIE1A); // Enable interrupts on channel 1A

	I2C_PORT_DIRECTION_REGISTER |= _BV(SDA_PIN) | _BV(SCL_PIN); // Set the SDA and SCL pins to be outputs
	I2C_PORT |= _BV(SDA_PIN) | _BV(SCL_PIN); // Set default as pins being high

	// Set Output Compare Register 1A (value the timer compares to to know when to reset)
	OCR1A = F_CPU / (PRESCALER * (BAUD_STD * 2));
	// Baud is multiplied by 2 so that one period is 100kHz in std mode

	sei(); // Reenable interrupts
}

int set_SDA(int bit) {
	while(globalTimerFlag);
	if (bit) {
		I2C_PORT |= _BV(SDA_PIN);
	} else {
		I2C_PORT &= ~_BV(SDA_PIN);
	}
	while(!globalTimerFlag);
}

int read_SDA() {
	while(!globalTimerFlag);
	return bit_is_set(I2C_PORT, SDA_PIN);
}

// This interrupt sets a global flag to be used by other functions (like setting the clock)
ISR(TIMERA_COMP_vect)
{
	// This just sets the clock pin
	if (gStartTimerFlag) {
		I2C_PORT ^= _BV(SCL_PIN);
		globalTimerFlag ^= 1;
	} else {
		I2C_PORT |= _BV(SCL_PIN);
	}

	if (globalTimerFlag && !control) {
		// Don't do anything becase SCL is high

		I2C_PORT_DIRECTION_REGISTER &= ~_BV(SDA_PIN);
		// unless you are reading ACK/NACK
	} else {
		
	}
}

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode) {
	I2C_PORT &= ~_BV(SDA_PIN);
	gStartTimerFlag = 1;
	set_SDA(1);

	int bit;

	for (int i = ADDRESS_LENGTH; i >= 0; i--) {
		bit = bit_is_set(secondary_address, i);
		set_SDA(bit);
	}

	set_SDA(mode);

	for (int i = 8; i >= 0; i--) {
		bit = bit_is_set(secondary_register, i);
		set_SDA(bit);
	}
}

int read_ACK_NACK(void) {
	while()
	int ack = bit_is_set(I2C_PORT, SDA_PIN); // Ack of 1 means acknowledged
	I2C_PORT_DIRECTION_REGISTER |= _BV(SDA_PIN);
	return ack;
}


void transmit_I2C(int msg[], int msg_length) {
	// for each byte in msg_length
	//  for i in 0:7
	//  if bit_is_set(msg, _BV(i)):
	//   Data line high // PORTB |= _BV(PB0)
	//  else
	//   Data line low
	//  Data line high
	//  Read ACK/NACK
	//  if NACK
	//  Try 10 times
	//   if unsuccessful stop
	// stop
	
	for (int i = 0; i < msg_length; i++) {
		for (int j = 0; j < 7; j++) {
			if (bit_is_set(msg, _BV(j))) {
				// set data line high
			} else {
				// set data line low
			}
		}	
	}
	// set dataline high
	
	int ERR = read_ACK_NACK();
	int try_again = 0;
	if (ERR == 1 && try_again < 10) {
		// try again
		try_again++;
	} else if (try_again == 10) {
		stop_I2C();
	} else {
		try_again = 0;
	}
}

void stop_I2C(void) {
	// set clockline high
	// set dataline high
}
