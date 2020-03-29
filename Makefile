MCU=atmega328p
F_CPU=16000000UL
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.
TARGET=i2c
SRCS=i2c.c
PROGRAMMER=usbasp
HELLOWORLDSRC=hello-world.c
HELLOWORLDTARGET=hello-world

all: libi2c.a
	${CC} ${CFLAGS} ${HELLOWORLDSRC} -L. -li2c -o ${HELLOWORLDTARGET}
	${OBJCOPY} -j .text -j .data -O ihex ${HELLOWORLDTARGET} ${HELLOWORLDTARGET}.hex

i2c.o: i2c.c i2c.h
	${CC} ${CFLAGS} -c ${SRCS} -o ${TARGET}.o

libi2c.a: i2c.o
	avr-ar rcs libi2c.a i2c.o

flash:
	avrdude -p ${MCU} -c ${PROGRAMMER} -U flash:w:${HELLOWORLDTARGET}.hex:i -F -P usb

clean:
	rm *.o *.hex *.a ${HELLOWORLDTARGET}
