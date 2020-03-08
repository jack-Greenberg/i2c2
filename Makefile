i2c.o: i2c.c i2c.h 
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o i2c.o i2c.c

i2c-compiled: i2c.o
	avr-gcc -mmcu=atmega328p i2c.o -o i2c

i2c.hex: i2c
	avr-objcopy -O ihex -R .eeprom i2c i2c.hex

flash: i2c.hex
	avrdude -F -V -c usbasp -p ATMEGA328P -U flash:w:i2c.hex

clean:
	rm *.o *.hex
