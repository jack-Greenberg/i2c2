#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU               16000000UL // Frequency of ATMega328p (16MHz)

#define BAUD_STD            100000
#define BAUD_FULL           400000
#define PRESCALER           1

#define ADDRESS_LENGTH      7

#define WRITE               0
#define READ                1

#define I2C_PORT_DIRECTION_REGISTER     DDRB
#define I2C_PORT                        PORTB
#define SDA_PIN                         PB4
#define SCL_PIN                         PB5

#define BIT_MASK 0x1;

volatile uint32_t globalTimerFlag = 0x01;
uint8_t gStartTimerFlag = 0;
uint8_t control = 0x01;

typedef struct Queue {
  int front, back, siz, capacity;
  uint8_t* arr;
} Queue;

Queue* make_queue(int capacity) {
  Queue* queue = malloc(sizeof(Queue));
  queue->capacity = capacity;
  queue->front = queue->siz = 0;
  queue->back = capacity - 1;
  queue->arr = malloc(sizeof(uint8_t) * queue->capacity);
  return queue;
}

int isFull(Queue* queue) 
{  return (queue->siz == queue->capacity);  } 
  
int isEmpty(Queue* queue) 
{  return (queue->siz == 0); } 

void enqueue(struct Queue* queue, uint8_t bit) 
{ 
    if (isFull(queue)) 
        return; 
    queue->back = (queue->back + 1) % queue->capacity; 
    queue->arr[queue->back] = bit; 
    queue->siz = queue->siz + 1; 
} 
  
// Function to remove an item from queue.  
// It changes front and size 
int dequeue(Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return 1; 
    int bit = queue->arr[queue->front]; 
    queue->front = (queue->front + 1) % queue->capacity; 
    queue->siz = queue->siz - 1; 
    return bit; 
}

int front(Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return 1; 
    return queue->arr[queue->front]; 
} 

Queue* bit_buffer = make_queue(32);

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

  gStartTimerFlag = 1;

  Serial.begin(9600);
}

int set_SDA(int bit) {
//  while(globalTimerFlag);
//  if (bit) {
//    I2C_PORT |= _BV(SDA_PIN);
//  } else {
//    I2C_PORT &= ~_BV(SDA_PIN);
//  }
  enqueue(bit_buffer, bit);
//  while(!globalTimerFlag);
}

int read_SDA() {
  while(!globalTimerFlag);
  return bit_is_set(I2C_PORT, SDA_PIN);
}

ISR(TIMER1_COMPA_vect)
{
  // This just sets the clock pin
  if (gStartTimerFlag) {
    I2C_PORT ^= _BV(SCL_PIN);
    globalTimerFlag ^= 1;
    int bit = front(bit_buffer);
    if (bit) {
      I2C_PORT |= _BV(SDA_PIN);
    } else {
      I2C_PORT &= ~_BV(SDA_PIN);
    }
    dequeue(bit_buffer);
  } else {
    I2C_PORT |= _BV(SCL_PIN);
  }

  if (globalTimerFlag && !control) {
    // Don't do anything becase SCL is high

    I2C_PORT_DIRECTION_REGISTER &= ~_BV(SDA_PIN);
    // unless you are reading ACK/NACK
  } else {
    
  }
}

void start_I2C(uint8_t secondary_address, uint8_t secondary_register, int mode) {
  set_SDA(0);
  gStartTimerFlag = 1;
  set_SDA(1);

  int bit;

  for (int i = ADDRESS_LENGTH; i >= 0; i--) {
    bit = bit_is_set(secondary_address, i);
    set_SDA(bit);
  }

  set_SDA(mode);

  for (int i = 8; i >= 0; i--) {
    bit = bit_is_set(secondary_register, i);
    set_SDA(bit);
  }
}

void loop() {
    enqueue(bit_buffer, 1);
    enqueue(bit_buffer, 0);
    enqueue(bit_buffer, 1);
    enqueue(bit_buffer, 0);
    enqueue(bit_buffer, 1);
    enqueue(bit_buffer, 0);
    enqueue(bit_buffer, 1);
    enqueue(bit_buffer, 0);enqueue(bit_buffer, 1);
    enqueue(bit_buffer, 0);enqueue(bit_buffer, 1);
    enqueue(bit_buffer, 0);
}
