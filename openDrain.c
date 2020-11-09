#include <avr/io.h>
#include "openDrain.h"

void setOpenDrainPin(uint8_t ddr, uint8_t port, uint8_t pin, opendrain_state_t state) {

	switch(state)
	{
	case OD_LOW:
		// setPinMode(port, pin, OUTPUT);
		ddr |= _BV(pin);
		// setOutput(port, pin, 0);
		port &= ~_BV(pin);
		break;
	case OD_HIGH:
		// setPinMode(port, pin, INPUT_PULLUP);
		ddr |= _BV(pin);
		port |= _BV(pin);
		break;
	case OD_HIGH_Z:
		// No pull-up in hi-z mode
		// setPinMode(port, pin, INPUT);
		ddr |= _BV(pin);
		port &= ~_BV(pin);
		break;
	}
}

void setOpenDrainHigh(uint8_t ddr, uint8_t port, uint8_t pin)
{
	setOpenDrainPin(ddr, port, pin, OD_HIGH);
}

void setOpenDrainLow(uint8_t ddr, uint8_t port, uint8_t pin)
{
	setOpenDrainPin(ddr, port, pin, OD_LOW);
}

void setOpenDrainHiZ(uint8_t ddr, uint8_t port, uint8_t pin)
{
	setOpenDrainPin(ddr, port, pin, OD_HIGH_Z);
}

void configureOpenDrainPin(opendrain_config_t config)
{
	switch(config)
	{
	case OD_CONFIG_NO_PULLUP:
		// setOpenDrainHiZ(port, pin);
		MCUCR |= _BV(PUD);
		break;
	case OD_CONFIG_PULLUP:
		// setOpenDrainHigh(port, pin);
		MCUCR &= ~_BV(PUD);
		break;
	}
}
