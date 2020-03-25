#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU               16000000UL // Frequency of ATMega328p (16MHz)

#define BAUD_STD            100000
#define BAUD_FULL           400000
#define PRESCALER           1

#define ADDRESS_LENGTH      2
#define REGISTER_LENGTH     2

#define WRITE               0
#define READ                1

#define I2C_PORT_DIRECTION_REGISTER     DDRB
#define I2C_PORT                        PORTB
#define SDA_PIN                         PB4
#define SCL_PIN                         PB5

#define BIT_MASK 0x1;

volatile uint8_t globalTimerFlag = 0x01;   // 1 means the clock is high, 0 means clock is low
volatile uint8_t gStartTimerFlag = 0;
uint8_t *startTimerFlagPtr = &gStartTimerFlag;
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
  OCR1A = (F_CPU / (PRESCALER * BAUD_STD * 2)) - 1;
  // Baud is multiplied by 2 so that one period is 100kHz in std mode

  sei(); // Reenable interrupts

//  gStartTimerFlag = 1;

//  Serial.begin(19200);
}

int set_SDA(int bit) {
  while(globalTimerFlag);
  if (bit) {
    I2C_PORT |= _BV(SDA_PIN);
  } else {
    I2C_PORT &= ~_BV(SDA_PIN);
  }
  while(!globalTimerFlag);
}

int read_SDA() {
  while(!globalTimerFlag);
  return bit_is_set(I2C_PORT, SDA_PIN);
}

ISR(TIMER1_COMPA_vect)
{
  if (gStartTimerFlag == 1) {
    I2C_PORT ^= _BV(SCL_PIN);
    globalTimerFlag ^= 1;
  } else {
    I2C_PORT |= _BV(SCL_PIN);
  }
}

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode) { 
  I2C_PORT &= ~_BV(SDA_PIN);
  I2C_delay();
  I2C_PORT &= ~_BV(SCL_PIN);
  globalTimerFlag = 0;
  I2C_delay();
//  I2C_PORT |= _BV(SDA_PIN);
  gStartTimerFlag = 1;
  
  // write the address
  for (int i = ADDRESS_LENGTH; i > 0; i--) {
    int cur_bit = bit_is_set(secondary_address, i - 1);
    set_SDA(cur_bit);
  }

  set_SDA(mode);
  // write the register
  for (int i = REGISTER_LENGTH; i > 0; i--) {
    int cur_bit = bit_is_set(secondary_register, i - 1);
    set_SDA(cur_bit);
  }
}

void stop_I2C() {
  I2C_PORT &= ~_BV(SDA_PIN);
  I2C_delay();
  gStartTimerFlag = 0;
  I2C_delay();
  I2C_PORT |= _BV(SCL_PIN);
  I2C_delay();
  I2C_PORT |= _BV(SDA_PIN);
}

void loop() {
  start_I2C(0x3, 0x1, 1); // 11101

  for (int i = 0; i < 10; i++)
  {
    __asm("nop");
  }

  stop_I2C();

  for (int i = 0; i < 100; i++)
  {
    __asm("nop");
  }
}
