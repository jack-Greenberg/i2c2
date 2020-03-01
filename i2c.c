#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

void init_I2C(int baud_rate)
{
	TCCR1A = _BV(WGM01);
	
	TCCR1B = _BV(CS10) | _BV(CS12);

	TIMSK1 = _BV(OCIE1A);

	OCR1A = 15624;	
}

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode) {
	// Start condition
	// // Pull down data line
	// // Start clock
	// // Release data line
	// // for each bit in secondary_address
	// // // transmit bit
	// // transmit mode
	// // read ack/nack
	// // // if nack, handle error (retry 10 times then cancel)
	// // Repeat for secondary register (with error handling)
}

void transmit_I2C(int msg[], int msg_length) {
	// for each byte in msg_length
	// // for i in 0:7
	// // // if bit_is_set(msg, _BV(i)):
	// // // // Data line high
	// // // else
	// // // // Data line low
	// // Data line high
	// // Read ACK/NACK
	// // if NACK
	// // // Try 10 times
	// // // // if unsuccessful stop
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
