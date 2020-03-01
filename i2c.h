#include <avr/io.h>

#define F_CPU 16000000UL // Frequency of ATMega328p (16MHz)

#define BAUD_STD 100000
#define BAUD_FULL 400000

#define WRITE 0
#define READ  1

void init_I2C(int baud_rate);

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode);

void write_I2C(uint8_t msg);

int read_ACK_NACK(void);

void stop_I2C(void);
