/**
 * I2C2
 * Implementation of I2C (Inter-Integrated Chip) protocol in C
 *
 * Authors: Jack Greenberg, David Tarazi
 * Date: 2020-04-03
 */

#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

#define BIT_MASK 0x1;

volatile uint32_t globalTimerFlag = 0x01;
uint8_t gStartTimerFlag = 0x00;
uint8_t internalTimerFlag = 0x00;

// Buffer for reading data from secondary
uint8_t read_buffer[MAX_READ_BYTES];

// EXAMPLE OF WHAT POINTER WOULD LOOK LIKE
// buffer pointer to start of read buffer to update the read buffer in function
// uint8_t *read_pointer = &read_buffer[0];

/**
 * Initializes timer and interrupts
 *
 * @param data_rate   transmission rate/frequency (STD is 100kHz, FAST is 400kHz)
 */
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
	I2C_PORT_DIRECTION_REGISTER |= _BV(SDA) | _BV(SCL);
	I2C_PORT |= _BV(SDA) | _BV(SCL);

	// Set Output Compare Register 1A (value the timer compares to to know when to reset)
	// Baud is multiplied by 2 so that one period is 100kHz in std mode
	OCR1A = F_CPU / (PRESCALER * (BAUD_STD * 2));

	// Reenable interrupts
	sei();
}

// Interrupt Service Routine for 'phantom clock' and SCL pin
ISR(TIMER1_COMPA_vect)
{
	// internalTimerFlag is a 'phantom clock' that is used for timing in the
	// start and stop conditions
	internalTimerFlag ^= 0x01;

	// If the timer should be active, then we flip SCL every time, as well as the
	// globalTimerFlag
	if (gStartTimerFlag) {
		I2C_PORT ^= _BV(SCL);
	}
}

/** 
 * Responsible for sending
 * - Start condition
 * - Address of secondary
 * - Mode (read/write)
 * - Register of secondary
 *
 * @param secondary_address    address of secondary device
 * @param secondary_register   register of secondary device
 * @param mode                 READ (1) or WRITE (0)
 */
void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode) {
	int ERR, i;

	/*** START CONDITION ***/
	
	// Stall while 'phantom timer' is low until it goes high
	while(!internalTimerFlag);

	// Pull down SDA while timer is high
	I2C_PORT &= ~_BV(SDA);

	// SCL should be high, so next time ISR is triggered, SCL will go low
	gStartTimerFlag = 1;

	/*** END OF START CONDITION ***/

	// Transmit address of secondary
	for (i = ADDRESS_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(secondary_address, i));
	}

	// Transmit I2C mode (read/write)
	set_SDA(mode);

	// Listen for ACK/NACK
	ERR = read_ACK_NACK();

	// Transmit register of secondary
	for (i = REGISTER_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(secondary_register, i));
	}

	ERR = read_ACK_NACK();
	I2C_PORT_DIRECTION_REGISTER |= _BV(SDA);
	if (ERR) {
		// TODO: Error handling
		// See above
	}
}

void repeated_start_I2C(uint8_t secondary_address, int mode) {
	/*** START CONDITION ***/
	
	// Stall while 'phantom timer' is low until it goes high
	while(!internalTimerFlag);

	// Pull down SDA while timer is high
	I2C_PORT &= ~_BV(SDA);

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
		I2C_PORT |= _BV(SDA);
	} else {
		I2C_PORT &= ~_BV(SDA);
	}

	// Listen for ACK/NACK
	int err = read_ACK_NACK();
	if (err) {
		// TODO: Error handling
		// Maybe put for loops inside a while loop which says `while (!err) {...`
		// so that ACK/NACK will actually do error handling	
	}
}

/**
 * Transmits one byte of information and performs error checking (ACK/NACK)
 *
 * @param msg   8 bit message to be sent
 */
void transmit_I2C(uint8_t msg) {
	int ERR, i;
	for (i = MSG_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(msg, i);
	}
	ERR = read_ACK_NACK();
	I2C_PORT_DIRECTION_REGISTER |= _BV(SDA);	
	while(bit_is_clear(I2C_PORT, SCL));
}
	
/**
 * Sets SDA line high or low
 *
 * @param bit   the bit to set SDA at
 */
void set_SDA(int bit) {
	// Stall until SCL is low
	while(bit_is_set(I2C_PORT, SCL));

	// If bit is 1, set SDA high, else set it low	
	if (bit) {
		I2C_PORT |= _BV(SDA);
	} else {
		I2C_PORT &= ~_BV(SDA);
	}

	// Stall until SCL is high again, then exit
	while(bit_is_clear(I2C_PORT, SCL));
}


void read_SDA(uint8_t secondary_address, uint8_t secondary_register, uint8_t *read_pointer, int bytes) {
	// reads bits from data line one byte at a time
	// read_pointer points to an element of an array of bytes
	// bytes is the number of bytes to read

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
		byte = (byte >> 7-i) | _BV(SDA);
		// Stall until SCL is low again
		while(globalTimerFlag);
	}
	return byte;
}


/**
 * Performs error checking after byte is transmitted
 *
 * @return   1 if ACK, 0 if NACK
 */ 
int read_ACK_NACK(void) {
	while(bit_is_set(I2C_PORT, SCL)); // Secondary is reading previous value

	// Give up control of SDA
	I2C_PORT_DIRECTION_REGISTER &= ~_BV(SDA);
	I2C_PORT &= ~_BV(SDA);

	// Read SDA
	return bit_is_clear(PINB, SDA);
	// returns 1 (truthy) if ACK
	// returns 0 (falsey) if NACK
}

void send_ACK(void) {
	// TODO: SEND AN ACK SIGNAL TO SECONDARY WHEN READING
}

void send_NACK(void) {
	// TODO: SEND A NACK SIGNAL TO SECONDARY IF ERROR WHEN
}

/**
 * Issues stop condition to secondary and stops SCL
 */
void stop_I2C(void) {
	/*** STOP CONDITION ***/

	// Stall while SCL is high until it goes low
	while(bit_is_set(I2C_PORT, SCL));

	// Pull down SDA
	I2C_PORT &= ~_BV(SDA);

	// Wait for timer to go high and then stop the timer
	while(bit_is_clear(I2C_PORT, SCL));
	gStartTimerFlag = 0;
	I2C_PORT |= _BV(SCL);

	// Set SDA back to high
	while(internalTimerFlag);
	I2C_PORT |= _BV(SDA);
}
