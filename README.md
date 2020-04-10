# I2C2

A light-weight I2C library written in C.

## Installation

```bash
$ git clone https://github.com/jack-greenberg/i2c2/

$ cd i2c2

$ make
```

## Usage

Edit `i2c.h` to update the `SDA` and `SCL` macros to match the pins on your MCU.

```C
#define SDA			PB2
#define SCL			PB3
```

Then, in your file, you can use the functions.

```C
int main(void) {
	init_I2C(BITRATE_STD); // Initializes timer and interrupts at 100kHz (use BITRATE_FAST for 400kHz)
	
	start_I2C(0x41, 0x81, WRITE); // Sends start condition and transmits address and register of secondary address

	transmit_I2C('H');
	transmit_I2C('e');
	// ...
	transmit_I2C('d');
	
	stop_I2C();

	return 0;
}

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)
