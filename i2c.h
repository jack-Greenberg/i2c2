#include <avr/io.h>
#include <util/delay.h>

#define F_CPU               16000000UL // Frequency of ATMega328p (16MHz)

#define BAUD_STD            100000
#define BAUD_FULL           400000
#define PRESCALER           1

#define ADDRESS_LENGTH      7

#define WRITE               0
#define READ                1

#define I2C_PORT_DIRECTION_REGISTER     DDRB
#define I2C_PORT                        PORTB
#define SDA_PIN                         PB2
#define SCL_PIN                         PB3

#define MAX_READ_BYTES      32

void init_I2C(int baud_rate);

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode);

void write_I2C(uint8_t msg);

int read_ACK_NACK(void);

void stop_I2C(void);
