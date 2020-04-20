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

      stop_I2C();

      // Chill for a bit
      for (int i = 0; i < 200; i++)
      {
        __asm("nop");
      }
    }
}
