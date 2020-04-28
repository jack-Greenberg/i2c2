/**
 * I2C2
 * Implementation of I2C (Inter-Integrated Chip) protocol in C
 *
 * Authors: Jack Greenberg, Manu Patil, David Tarazi
 * Date: 2020-04-03
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"

#define BIT_MASK 0x1;

volatile uint32_t globalTimerFlag = 0x01;
uint8_t gStartTimerFlag = 0x00;
volatile uint8_t internalTimerFlag = 0x00;

/**
 * Initializes timer and interrupts
 *
 * @param data_rate   transmission rate/frequency (STD is 100kHz, FAST is 400kHz)
 */
void init_I2C(unsigned long bitrate)
{
	// Stop interrupts and reset Timer 1
	cli();
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	TIMSK0 = 0;

	// PWM, phase correct, TOP is OCR1A
	TCCR1A |= _BV(WGM10) | _BV(WGM11);
	TCCR1B |= _BV(WGM13);

	// Enable interrupts
	TIMSK0 |= _BV(OCIE1A);

    // Clock prescaler 1
	TCCR1B |= _BV(CS10); // NEED TO COMMUNICATE PRESCALARS (SHOW DISCRETE POSSIBLITIES)

	// Set Output Compare Register 1A (value the timer compares to to know when to reset)
	OCR1A = F_CPU / (PRESCALER * (bitrate * 4));

	// Set the SDA and SCL pins to be outputs
	// Set pins to be high at the beginning
	I2C_PORT_DIRECTION_REGISTER = 0;
	I2C_PORT = 0;

	DDRB &= ~_BV(SDA); // & ~_BV(SCL);
	DDRB |= _BV(SCL);

	// Reenable interrupts
	sei();
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

	// Pull down SDA while SCL is high
	DDRB |= _BV(SDA);

	// Start timer
	TCCR1A |= _BV(COM1A0); // Clear OC1A on compare match when up-counting. Set OC1A on compare match when down-counting.

	while(bit_is_set(PINB, SCL));

	/*** END OF START CONDITION ***/

	// Transmit address of secondary
	for (i = ADDRESS_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(secondary_address, i));
	}

	// Transmit I2C mode (read/write)
	set_SDA(mode);

	while(bit_is_set(PINB, SCL)); // Wait while secondary is reading SDA
	DDRB &= ~_BV(SDA);
	while(bit_is_clear(PINB, SCL));
	ERR = bit_is_set(PINB, SDA);
	// while(bit_is_set(PINB, SCL));

	// Transmit register of secondary
	for (i = REGISTER_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(secondary_register, i));
	}

	while(bit_is_set(PINB, SCL)); // Wait while secondary is reading SDA
	DDRB &= ~_BV(SDA);
	while(bit_is_clear(PINB, SCL));
	ERR = bit_is_set(PINB, SDA);
	// while(bit_is_set(PINB, SCL));
}

/**
 * Transmits one byte of information and performs error checking (ACK/NACK)
 *
 * @param msg   8 bit message to be sent
 */
void transmit_I2C(uint8_t msg) {
	int ERR, i;
	for (i = MSG_LENGTH - 1; i >= 0; i--) {
		set_SDA(bit_is_set(msg, i));
	}

	while(bit_is_set(PINB, SCL)); // Wait while secondary is reading SDA
	DDRB &= ~_BV(SDA);
	while(bit_is_clear(PINB, SCL));
	ERR = bit_is_set(PINB, SDA);
	// while(bit_is_set(PINB, SCL));

}

/**
 * Sets SDA line high or low
 *
 * @param bit   the bit to set SDA at
 */
void set_SDA(int bit) {
	// Stall until SCL is low
	while(bit_is_set(PINB, SCL));

	// If bit is 1, set SDA high, else set it low
	if (bit) {
		DDRB &= ~_BV(SDA);
	} else {
		DDRB |= _BV(SDA);
	}

	// Stall until SCL is high again, then exit
	while(bit_is_clear(PINB, SCL));
}


// void read_SDA(uint8_t secondary_address, uint8_t secondary_register, uint8_t *read_pointer, int bytes) {
// 	// reads bits from data line one byte at a time
// 	// read_pointer points to an element of an array of bytes
// 	// bytes is the number of bytes to read
//
// 	// begin the I2C protocol in write mode
// 	start_I2C(secondary_address, secondary_register, 0);
//
// 	// create a repeated start to start i2c in read mode
// 	repeated_start_I2C(secondary_address, 1);
//
// 	// count tracks how many bytes we've read
// 	int count = 0;
//
// 	for (int i = 0; i < bytes; i++) {
// 		// get a byte and shift the buffer location over
// 		*read_pointer = get_byte();
//
// 		// error with trying to read more bytes than the buffer can handle
// 		if (count >= bytes) {
// 			// TODO: SEND NACK BIT HERE
// 			stop_I2C();
// 		}
//
// 		// TODO: SEND ACK BIT HERE
//
// 		// update pointer location and count
// 		read_pointer++;
// 		count++;
// 	}
// 	stop_I2C();
// }
//
// uint8_t get_byte(void) {
// 	// reads one byte and returns it
// 	uint8_t byte = 0;
// 	for (int i = 0; i < 8; i++) {
// 		// Stall until SCL is high
// 		while(!globalTimerFlag);
// 		// set the byte
// 		byte = (byte >> (7-i)) | _BV(SDA);
// 		// Stall until SCL is low again
// 		while(globalTimerFlag);
// 	}
// 	return byte;
// }


/**
 * Performs error checking after byte is transmitted
 *
 * @return   1 if ACK, 0 if NACK
 */
int read_ACK_NACK(void) {
	while(bit_is_set(PINB, SCL)); // Secondary is reading previous value

	// Give up control of SDA, disable internal pull-up resistors
	DDRB |= _BV(SDA);
	PORTB |= _BV(SDA);

	// while(bit_is_clear(PINB, SCL));

	// Read SDA
	// return 0;
	return bit_is_clear(PINB, SDA);
}

// void send_ACK(void) {
// 	// TODO: SEND AN ACK SIGNAL TO SECONDARY WHEN READING
// }
//
// void send_NACK(void) {
// 	// TODO: SEND A NACK SIGNAL TO SECONDARY IF ERROR WHEN
// }

/**
 * Issues stop condition to secondary and stops SCL
 */
void stop_I2C(void) {
	/*** STOP CONDITION ***/

    // Wait for previous read_ACK_NACK() to be read
	while(bit_is_clear(PINB, SCL));
	while(bit_is_set(PINB, SCL));

	// Pull down SDA
	DDRB |= _BV(SDA);

    // Stop timer
	TCCR1A &= ~_BV(COM1A0);

	DDRB &= ~_BV(SDA);

	// Set SCL high
	I2C_PORT |= _BV(SCL);
}
