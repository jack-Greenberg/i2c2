#include <avr/io.h>

typedef enum
{
	OD_LOW,
	OD_HIGH,
	OD_HIGH_Z,
} opendrain_state_t;


typedef enum
{
	OD_CONFIG_NO_PULLUP,
	OD_CONFIG_PULLUP,
} opendrain_config_t;


void setOpenDrainPin(uint8_t ddr, uint8_t port, uint8_t pin, opendrain_state_t state);

// Helpers for above function
void setOpenDrainHigh(uint8_t ddr, uint8_t port, uint8_t pin);
void setOpenDrainLow(uint8_t ddr, uint8_t port, uint8_t pin);
void setOpenDrainHiZ(uint8_t ddr, uint8_t port, uint8_t pin);

void configureOpenDrainPin(opendrain_config_t config);
