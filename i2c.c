#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

#define BIT_MASK 0x1;

volatile uint32_t globalTimerFlag = 0x01;
uint8_t gStartTimerFlag = 0x00;
uint8_t internalTimerFlag = 0x00;
uint8_t control = 0x01;

// buffer for reading data from secondary
uint8_t read_buffer[MAX_READ_BYTES];

// EXAMPLE OF WHAT POINTER WOULD LOOK LIKE
// buffer pointer to start of read buffer to update the read buffer in function
// uint8_t *read_pointer = &read_buffer[0];

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

	// Set the SDA and SCL pins to be outputs
	// Set pins to be high at the beginning
	I2C_PORT_DIRECTION_REGISTER |= _BV(SDA_PIN) | _BV(SCL_PIN);
	I2C_PORT |= _BV(SDA_PIN) | _BV(SCL_PIN);

	// Set Output Compare Register 1A (value the timer compares to to know when to reset)
	// Baud is multiplied by 2 so that one period is 100kHz in std mode
	OCR1A = F_CPU / (PRESCALER * (BAUD_STD * 2));

	// Reenable interrupts
	sei();
}

// This interrupt sets a global flag to be used by other functions (like setting the clock)
ISR(TIMER1_COMPA_vect)
{
	// internalTimerFlag is a 'phantom clock' that is used for timing in the
	// start and stop conditions
	internalTimerFlag ^= 0x01;

	// If the timer should be active, then we flip SCL every time, as well as the
	// globalTimerFlag
	if (gStartTimerFlag) {
		I2C_PORT ^= _BV(SCL_PIN);
		globalTimerFlag ^= 1;
	}
}


void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode) {
	/*** START CONDITION ***/
	
	// Stall while 'phantom timer' is low until it goes high
	while(!internalTimerFlag);

	// Pull down SDA while timer is high
	I2C_PORT &= ~_BV(SDA_PIN);

	// SCL should be high, so next time ISR is triggered, SCL will go low
	globalTimerFlag = 1;
	gStartTimerFlag = 1;

	/*** END OF START CONDITION ***/

	// Transmit address of secondary
	for (int i = ADDRESS_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(secondary_address, i));
	}

	// Transmit I2C mode (read/write)
	// (Inlined because calling set_SDA() had timing issues)
	while(globalTimerFlag);
	if (mode) {
		I2C_PORT |= _BV(SDA_PIN);
	} else {
		I2C_PORT &= ~_BV(SDA_PIN);
	}

	// Listen for ACK/NACK
	int err = read_ACK_NACK();
	if (err) {
		// TODO: Error handling
		// Maybe put for loops inside a while loop which says `while (!err) {...`
		// so that ACK/NACK will actually do error handling	
	}

	// Transmit register of secondary
	for (int i = REGISTER_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(secondary_register, i));
	}

	err = read_ACK_NACK();
	if (err) {
		// TODO: Error handling
		// See above
	}
}

void repeated_start_I2C(uint8_t secondary_address, int mode) {
	/*** START CONDITION ***/
	
	// Stall while 'phantom timer' is low until it goes high
	while(!internalTimerFlag);

	// Pull down SDA while timer is high
	I2C_PORT &= ~_BV(SDA_PIN);

	// SCL should be high, so next time ISR is triggered, SCL will go low
	globalTimerFlag = 1;
	gStartTimerFlag = 1;

	/*** END OF START CONDITION ***/

	// Transmit address of secondary
	for (int i = ADDRESS_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(secondary_address, i));
	}

	// Transmit I2C mode (read/write)
	// (Inlined because calling set_SDA() had timing issues)
	while(globalTimerFlag);
	if (mode) {
		I2C_PORT |= _BV(SDA_PIN);
	} else {
		I2C_PORT &= ~_BV(SDA_PIN);
	}

	// Listen for ACK/NACK
	int err = read_ACK_NACK();
	if (err) {
		// TODO: Error handling
		// Maybe put for loops inside a while loop which says `while (!err) {...`
		// so that ACK/NACK will actually do error handling	
	}
}


void transmit_I2C(uint8_t secondary_address, uint8_t secondary_register, int msg[], int msg_length) {
	// write data over SDA

	// begin the I2C protocol in write mode
	start_I2C(secondary_address, secondary_register, 0);

	int attempts;
	int ERR;

	// loop through message
	for (int i = 0; i < msg_length; i++) {
		attempts = 0;
		// attempt 10 times if there is an error
		do {
			for (int j = 0; j < 7; j++) {
				if (bit_is_set(msg, _BV(j))) {
					// set data line high
					set_SDA(1);
				} else {
					// set data line low
					set_SDA(0);
				}
			}
			ERR = read_ACK_NACK();
			attempts++;	
		} while (ERR && attempts < 10);

		if (ERR) {
			// attempted 10 times and error still holds true
			stop_I2C();
			break;
		}
	}
	// finish I2C protocol
	stop_I2C();
}
	

int set_SDA(int bit) {
	// Stall until SCL is low
	while(globalTimerFlag);

	// If bit is 1, set SDA high, else set it low	
	if (bit) {
		I2C_PORT |= _BV(SDA_PIN);
	} else {
		I2C_PORT &= ~_BV(SDA_PIN);
	}

	// Stall until SCL is high again, then exit
	while(!globalTimerFlag);
}


void read_SDA(uint8_t secondary_address, uint8_t secondary_register, uint8_t *read_pointer, int bytes) {
	// reads bits from data line one byte at a time
	// read_pointer points to an element of an array of bytes
	// bytes is the number of bytes to read
	// returns 0 for failure, 1 for success

	// begin the I2C protocol in write mode
	start_I2C(secondary_address, secondary_register, 0);

	// create a repeated start to start i2c in read mode
	repeated_start_I2C(secondary_address, 1);

	// count tracks how many bytes we've read
	int count = 0;

	for (int i = 0; i < bytes; i++) {
		// get a byte and shift the buffer location over
		*read_pointer = get_byte();

		// error with trying to read more bytes than the buffer can handle
		if (count >= bytes) {
			// TODO: SEND NACK BIT HERE
			stop_I2C();
		}

		// TODO: SEND ACK BIT HERE

		// update pointer location and count
		read_pointer++;
		count++;
	}
	stop_I2C();
}

uint8_t get_byte(void) {
	// reads one byte and returns it
	uint8_t byte = 0;
	for (int i = 0; i < 8; i++) {
		// Stall until SCL is high
		while(!globalTimerFlag);
		// set the byte
		byte = (byte >> 7-i) | _BV(SDA_PIN);
		// Stall until SCL is low again
		while(globalTimerFlag);
	}
	return byte;
}


// ERROR HANDLING WITH ACK/NACK BIT
int read_ACK_NACK(void) {
	/* while() */
	int ack = bit_is_set(I2C_PORT, SDA_PIN); // Ack of 1 means acknowledged
	// Or should it be reversed, like Unix (0 is OK, 1 is err...)
	// That is how it is for the start_I2C function right now...
	I2C_PORT_DIRECTION_REGISTER |= _BV(SDA_PIN);
	return ack;
}

void send_ACK(void) {
	// TODO: SEND AN ACK SIGNAL TO SECONDARY WHEN READING
}

void send_NACK(void) {
	// TODO: SEND A NACK SIGNAL TO SECONDARY IF ERROR WHEN
}


void stop_I2C(void) {
	/*** STOP CONDITION ***/

	// Stall while SCL is high until it goes low
	while(globalTimerFlag);

	// Pull down SDA
	I2C_PORT &= ~_BV(SDA_PIN);

	// Wait for timer to go high and then stop the timer
	while(!globalTimerFlag);
	gStartTimerFlag = 0;
	I2C_PORT |= _BV(SCL_PIN);

	// Set SDA back to high
	while(internalTimerFlag);
	I2C_PORT |= _BV(SDA_PIN);
}
