#include "i2c.h"

int main(void)
{
	init_I2C(BAUD_STD);

	while(1) {
		start_I2C(0x41, 0x81, WRITE);

		transmit_I2C('H');
		transmit_I2C('e');
		transmit_I2C('l');
		transmit_I2C('l');
		transmit_I2C('o');
		transmit_I2C(' ');
		transmit_I2C('w');
		transmit_I2C('o');
		transmit_I2C('r');
		transmit_I2C('l');
		transmit_I2C('d');

		stop_I2C();

		// Wait for a bit...
		for (int i = 0; i < 100; i++) {
			__asm("nop");
		}
	}

	return 0;
}
