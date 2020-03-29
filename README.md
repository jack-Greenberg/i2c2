# I2C2 (I2C in C)
Jack Greenberg and David Tarazi

## Links
* [Trello](https://trello.com/b/Ed0RLSGK/i2c2)

## Functions
* `init_I2C()`
	* Initializes the timer and interrupts
* `start_I2C()`
	* Sends __1.__ start condition, __2.__ address of secondary, __3.__ mode (read/write), and __4.__ register of secondary
* `transmit_I2C()`
	* Responsible for sending one byte of data via I2C and performing error handling (ACK/NACK).
* `stop_I2C()`
	* Sends stop condition to terminate communication

## Reports
* [Initial proposal](https://github.com/jack-Greenberg/SoftSysI2C2/tree/master/reports/proposal.md)
* [Project update](https://github.com/jack-Greenberg/SoftSysI2C2/blob/master/reports/update.md)
* <span style="color: #888">__Final report...__</span>

## Resources
* [Understanding the I2C Bus](https://www.ti.com/lit/an/slva704/slva704.pdf) - Texas Instruments, 2015
* [ATmega 328P Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf) - Atmel
* [I2C Bus Pullup Resistor Calculation](http://www.ti.com/lit/an/slva689/slva689.pdf) - Texas Instruments, 2015
* [Newbie's Guide to AVR Timers](https://github.com/jack-Greenberg/SoftSysI2C2/blob/master/resources/Timers.pdf) - Dean Camera, 2016
