# I2C2 (I2C in C)
### David Tarazi and Jack Greenberg

## Links
* [Trello](https://trello.com/b/Ed0RLSGK/i2c2)
* 

## Functions
* `void initI2C(void)`
	* Initializes device by assigning the correct pins and registers, as well as initializing clock rates

* `startI2C`
	* Send start condition and address
	* Slave address as arg

* `transmit`

* `readAckNack`

* `stop`

* `read`

## Programming Arduino

1. Generate object code
```
avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o file.o file.c
```
2. Finish compiling
```
avr-gcc -mmcu=atmega328p file.o -o file
```
3. Convert to hex code
```
avr-objcopy -O ihex -R .eeprom file file.hex
```
4. Flash with usbasp
```
avrdude -F -V -c usbasp -p ATMEGA328P -U flash:w:file.hex
```
