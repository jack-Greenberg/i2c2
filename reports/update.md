# I2C2 (I2C in C)
### David Tarazi and Jack Greenberg

The goal of our project is to write an implementation of the I2C (inter-integrated chip) protocol in C. Our MVP will be having two Arduinos communicate over I2C (sending random data or sensor data from another I2C). We will manipulate the pins and ports of the Arduino to enable device communication, and have some sort of sensor (like a soil moisture sensor) that produces data and sends it via I2C to our Arduino, and we will then be able to read out from the Arduino using a serial output.

I2C is a *two wire interface*, *physical layer* protocol that has a clock line and a data line. The advantage of I2C is that you can have a single *primary* device that controls the bus with up to 128 secondary devices, all just using two wires. I2C is one of the most commonly used microchip communication protocols.

### Goals
Our goals are learning about writing embedded firmware and about inter-device communications at the physical level. We also hope to learn about reading data sheets and understanding the technical jargon they contain. As we worked more with the Arduino and its functionality at a lower level, we now hope to learn more about communications between multiple devices and develop multiple ICs working with the same communication protocol. We will eventually work towards sending messages from one Arduino to another and controlling their status. We want to learn more about how to interface with that and be able to type a message into our computer and read it on another computer via serial and I2C.

So far, we have had a lot of experience reading technical descriptions, white pages, and data sheets for things related to I2C. This involves a [Texas Instrument report on the I2C bus](https://www.ti.com/lit/an/slva704/slva704.pdf), another [Texas Instruments paper about resistor value calculations](http://www.ti.com/lit/an/slva689/slva689.pdf), and the [data sheet for the ATMega328p](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf), which is the microcontroller used by Arduino. The next big concept we will be tackling is the issue of synchronization of clock signals (see **Next Steps**).

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

---

[Trello link](https://trello.com/b/Ed0RLSGK/i2c2)

[GitHub Repository](https://github.com/jack-Greenberg/SoftSysI2C2)
