#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"

int main()
{
    init_I2C(BITRATE_STD);
}
