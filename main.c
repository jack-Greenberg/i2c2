#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"

int main()
{
    init_I2C(BITRATE_STD);

    // start_timer();

    while (1) {
      // start_timer();
      start_I2C(0x41, 0x81, WRITE);

      // for (int i = 0; i < 200; i++)
      // {
      //   __asm("nop");
      // }
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

      // Chill for a bit
      for (int i = 0; i < 500; i++)
      {
        __asm("nop");
      }
    }
}
