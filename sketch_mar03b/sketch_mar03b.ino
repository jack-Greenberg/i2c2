#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU               16000000UL // Frequency of ATMega328p (16MHz)

#define FREQ_STD            100
#define FREQ_FULL           400000
#define PRESCALER           1

#define ADDRESS_LENGTH      7
#define REGISTER_LENGTH     8

#define WRITE               0x0
#define READ                0x1

#define MAX_READ_BYTES      32

#define I2C_PORT_DIRECTION_REGISTER     DDRB
#define I2C_PORT                        PORTB
#define SDA_PIN                         PB4
#define SCL_PIN                         PB5

#define BIT_MASK 0x1;

uint8_t read_buffer[MAX_READ_BYTES];

volatile uint8_t globalTimerFlag = 0x01;   // 1 means the clock is high, 0 means clock is low
volatile uint8_t gStartTimerFlag = 0x00;
volatile uint8_t internalTimerFlag = 0x00;
uint8_t control = 0x01;

void I2C_delay(void) {}

void setup() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  TCCR1B |= _BV(WGM12); // CTC mode
  TCCR1B |= _BV(CS10); // PRESCALER 1
  TIMSK1 |= _BV(OCIE1A); // Enable interrupts on channel 1A

  I2C_PORT_DIRECTION_REGISTER |= _BV(SCL_PIN) | _BV(SDA_PIN); // Set the SDA and SCL pins to be outputs
  I2C_PORT |= _BV(SDA_PIN) | _BV(SCL_PIN); // Set default as pins being high

  // Set Output Compare Register 1A (value the timer compares to to know when to reset)
  OCR1A = (F_CPU / (PRESCALER * FREQ_STD * 2)) - 1;
  // Baud is multiplied by 2 so that one period is 100kHz in std mode

  sei(); // Reenable interrupts

//  Serial.begin(19200);
}

int set_SDA(uint8_t b) {
  while(globalTimerFlag);
  if (b) {
    I2C_PORT |= _BV(SDA_PIN);
  } else {
    I2C_PORT &= ~_BV(SDA_PIN);
  }
  while(!globalTimerFlag);
}

ISR(TIMER1_COMPA_vect)
{
  internalTimerFlag ^= 0x1;
  if (gStartTimerFlag == 1) {
    globalTimerFlag ^= 0x1;
    I2C_PORT ^= _BV(SCL_PIN);
  }
}

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode) {
  /*** START CONDITION ***/
  
  // Stall while internal timer is low until it goes high
  while(!internalTimerFlag);

  // Pull down SDA while SCL is high
  I2C_PORT &= ~_BV(SDA_PIN);

  // SCL should be high, so next time ISR is triggered, SCL will go low
  globalTimerFlag = 1;
  gStartTimerFlag = 1;

  /*** END OF START CONDITION ***/

  // Write the address
  for (int i = ADDRESS_LENGTH - 1; i >= 0; i--) {
    set_SDA(bit_is_set(secondary_address, i));
  }

  // Write the mode
  while(globalTimerFlag);
  if (mode) {
    I2C_PORT |= _BV(SDA_PIN);
  } else {
    I2C_PORT &= ~_BV(SDA_PIN);
  }

  int ERR = read_ACK_NACK();
  
  // Write the register
  for (int i = REGISTER_LENGTH - 1; i >= 0; i--) {
    set_SDA(bit_is_set(secondary_register, i));
  }

  ERR = read_ACK_NACK();

  int msg = 0x81;
  for (int i = 8 - 1; i >= 0; i--) {
    set_SDA(bit_is_set(msg, i));
  }

  ERR = read_ACK_NACK();

  msg = 0x81;
  for (int i = 8 - 1; i >= 0; i--) {
    set_SDA(bit_is_set(msg, i));
  }

  ERR = read_ACK_NACK();

  stop_I2C();
}

void transmit_I2C(uint8_t secondary_address, uint8_t secondary_register, int msg[], int msg_length) {
        // write data over SDA

        // begin the I2C protocol in write mode
        start_I2C(secondary_address, secondary_register, WRITE);

        int attempts;
        int ERR;

        // loop through message
        for (int i = 0; i < msg_length; i++) {
                attempts = 0;
                // attempt 10 times if there is an error
                do {
                        for (int j = 0; j < 7; j++) {
                                set_SDA(bit_is_set(msg, j));
                        }
//                        ERR = read_ACK_NACK();
                        ERR = 0;
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


int read_ACK_NACK(void) {
//  while(!globalTimerFlag);
  // If the line was pulled down
  if (bit_is_clear(PINB, SDA_PIN)) {
    // Return success
    return 1;
  } else {
    // Return failure (falsy)
    return 0;
  }

  while(globalTimerFlag);
}


void stop_I2C() {
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

void loop() {
  start_I2C(0x41, 0x81, WRITE); // 11101

//  stop_I2C();

//  int msg[] = {'A', 'B', 'C'};
//  transmit_I2C(0x35, 0x49, msg, 3);

  // Chill for a bit
  for (int i = 0; i < 100; i++)
  {
    __asm("nop");
  }
}
