# I2C2 (I2C in C)
## David Tarazi and Jack Greenberg

The goal of our project is to write an implementation of the I2C (inter-integrated chip) protocol in C. Our MVP will be having two Arduinos communicate over I2C (sending random data or sensor data from another I2C). We will manipulate the pins and ports of the Arduino to enable device communication, and have some sort of sensor (like a soil moisture sensor) that produces data and sends it via I2C to our Arduino, and we will then be able to read out from the Arduino using a serial output.

### Goals
Our goals are learning about writing embedded firmware and about inter-device communications at the physical level. We also hope to learn about reading data sheets and understanding the technical jargon they contain.

### Getting Started
We started off by doing research about I2C and setting up Arduinos to develop the project. We found resources for working with timers in AVR chips (Arduino uses an AVR chip called the ATMega328p with a 16MHz clock timer). We ran calculations to figure out the right set up so that the clock can operate at 100kHz and confirmed our set up with an **Analog Discovery** oscilloscope. 

We also wrote pseudo-code for all the functions that I2C uses, including

* init_I2C()
* start_I2C()
* transmit()
* read()
* read_ack_nack()
* set_SDA()

### Next Steps

The next step is to solve an issue we came across where the data line isn't operating at the correct frequency. This is likely due to the fact that the data line (SDA) operates based on the state of the clock line (SCL) but they are often transmitting different states (high vs low).

The three concrete tasks for the next phase are:

1. Find a way to synchronize SCL and SDA (Jack). This will be "done" when we can see the clock line and the data line operating at the same frequency. The data line should only be changing while the clock line is low, and if we are just flipping the data line between high and low, the high and low states should be present for the same amount of time.
2. Write functions for reading SDA as a secondary (slave) device (David). For this to be considered done, we should have established a working serial connection between two arduinos and have code that is written for the receiver to read at the correct times assuming the issue we have with SCL and SDA communication synchronization is resolved.
3. Calculate resistor values for I2C circuit (Jack). This will be done when we have a final resistor value based on [Texas Instruments' guide](http://www.ti.com/lit/an/slva689/slva689.pdf).