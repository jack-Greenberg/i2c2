# I2C2

A light-weight I2C library written in C.

## Installation

```bash
$ git clone https://github.com/jack-greenberg/i2c2.git
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
	init_I2C(BITRATE_STD); // Initializes timer and interrupts at 100kHz (use BITRATE_FULL for 400kHz)
	
	start_I2C(0x41, 0x81, WRITE); // Sends start condition and transmits address and register of secondary address

	transmit_I2C('H');
	transmit_I2C('e');
	// ...
	transmit_I2C('d');
	
	stop_I2C();

	return 0;
}
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## To-Do

- [x] Transmit from primary to secondary
- [ ] Implement break when read_ACK_NACK() returns NACK
- [ ] Read from secondary on primary
  - [ ] Implement repeated start
  - [ ] read_I2C()
  - [ ] send_ACK()
  - [ ] send_NACK()
- [ ] Set up clearer guidelines about timers
- [ ] Allow for using Timer0 or Timer1
- [ ] Add documentation of results from Analog Discovery
- [ ] Add more details to **Usage**
- [ ] Make Makefile more flexible

## License
[MIT](https://choosealicense.com/licenses/mit/)
