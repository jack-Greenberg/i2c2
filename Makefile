MCU=atmega328p
F_CPU=16000000UL
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.
TARGET=main
SRCS=main.c
PROGRAMMER=arduino

flash: main.hex
	avrdude -p ${MCU} -c ${PROGRAMMER} -U flash:w:${TARGET}.hex:i -F -P /dev/ttyACM0

main.hex: i2c.o libi2c.a main.o
	${CC} ${CFLAGS} ${SRCS} -L. -li2c -o ${TARGET}.elf
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.elf ${TARGET}.hex

libi2c.a: i2c.o
	avr-ar rcs libi2c.a i2c.o

i2c.o: i2c.c i2c.h
	${CC} ${CFLAGS} -c i2c.c -o i2c.o

.PHONY: clean
clean:
	rm *.o *.hex *.a *.elf
