#include <avr/io.h>
#include <util/delay.h>

#define F_CPU               16000000UL // Frequency of ATMega328p (16MHz)

#define BAUD_STD            100000
#define BAUD_FULL           400000
#define PRESCALER           1

#define ADDRESS_LENGTH      7

#define WRITE               0
#define READ                1

#define MSG_LENGTH			8 // 8 bits or 1 byte

#define I2C_PORT_DIRECTION_REGISTER     DDRB
#define I2C_PORT                        PORTB
#define SDA								PB2 // The pin on the AVR chip
#define SCL		                        PB3 // The pin on the AVR chip

#define MAX_READ_BYTES      32

void init_I2C(int baud_rate);

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode);
void repeated_start_I2C(uint8_t secondary_address, int mode);

void transmit_I2C(int msg);
int set_SDA(int bit);

void read_SDA(uint8_t secondary_address, uint8_t secondary_register, uint8_t *read_pointer, int bytes);
uint8_t get_byte(void);

int read_ACK_NACK(void);
void send_ACK(void);
void send_NACK(void);

void stop_I2C(void);
