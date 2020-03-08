#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU               16000000UL // Frequency of ATMega328p (16MHz)

#define BAUD_STD            100000
#define BAUD_FULL           400000
#define PRESCALER           1

#define ADDRESS_LENGTH      7
#define REGISTER_LENGTH     8

#define WRITE               0
#define READ                1

#define I2C_PORT_DIRECTION_REGISTER     DDRB
#define I2C_PORT                        PORTB
#define SDA_PIN                         PB4
#define SCL_PIN                         PB5

#define BIT_MASK 0x1;

volatile uint8_t globalTimerFlag = 0x01;   // 1 means the clock is high, 0 means clock is low
uint8_t gStartTimerFlag = 0;
uint8_t control = 0x01;

void setup() {
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
  OCR1A = (F_CPU / (PRESCALER * BAUD_STD * 2)) - 1;
  // Baud is multiplied by 2 so that one period is 100kHz in std mode

  sei(); // Reenable interrupts
<<<<<<< HEAD
=======

>>>>>>> caafdf4a32a0a520c3761bbb12b96094bac38268
  gStartTimerFlag = 1;

  Serial.begin(19200);
}

int set_SDA(int cur_bit) {
  while(globalTimerFlag);       // stall until clock is low
  if (cur_bit) {
    I2C_PORT |= _BV(SDA_PIN);
  } else {
    I2C_PORT &= ~_BV(SDA_PIN);
  }
  // while(!globalTimerFlag);      // stall until clock goes high again
}

int read_SDA() {
  while(!globalTimerFlag);
  return bit_is_set(I2C_PORT, SDA_PIN);
}

ISR(TIMER1_COMPA_vect)
{
  // flip clock state if in I2C routine
  if (gStartTimerFlag) {
    I2C_PORT ^= _BV(SCL_PIN);
    globalTimerFlag ^= 1;
  } else {
    I2C_PORT |= _BV(SCL_PIN);
  }

//  if (globalTimerFlag && !control) {
//    // Don't do anything becase SCL is high
//
//    I2C_PORT_DIRECTION_REGISTER &= ~_BV(SDA_PIN);
//    // unless you are reading ACK/NACK
//  } else {
//    
//  }
}

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode) {
  // wait until clock line is high then set data line to low, then initiate start condition
  while(!globalTimerFlag);
  I2C_PORT &= ~_BV(SDA_PIN);
  gStartTimerFlag = 1;

  int cur_bit;
  // write the address
  for (int i = ADDRESS_LENGTH; i > 0; i--) {
    cur_bit = bit_is_set(secondary_address, i);
    set_SDA(cur_bit);
  }

  set_SDA(mode);
  // write the register
  for (int i = REGISTER_LENGTH; i > 0; i--) {
    cur_bit = bit_is_set(secondary_register, i);
    set_SDA(cur_bit);
  }
}

void loop() {
    set_SDA(1);
    set_SDA(0);
}
