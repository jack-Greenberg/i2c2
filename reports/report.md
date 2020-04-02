# Implementing I2C in C with Arduino
Jack Greenberg and David Tarazi

## Project Goals

## Learning Goals

### Jack

My main goal was to become more comfortable writing and debugging firmware, and this project was perfect because it forced me to learn how to debug code without having much feedback about works and what doesn't. I learned a lot about how AVR timers work and about manipulating bits. One of the coolest things I learned during the project was that initializing a variable carries a significant time delay when operating at the microsecond scale.

## David

*David's goals go here*



## Useful Resources

The main resource we used for this project was a document from Texas Instruments entitled [*Understanding the I<sup>2</sup>C Bus*](https://www.ti.com/lit/an/slva704/slva704.pdf). The document details exactly how the I2C protocol works, including a range of topics from the details of the open-drain/open-collector electrical system to transmitting addresses and performing simple error checking with ACK/NACK.

We also used a document called [I2C Bus Pullup Resistor Calculation](http://www.ti.com/lit/an/slva689/slva689.pdf) (also from Texas Instruments) that explains the mathematics and physics behind choosing pull-up resistors to add to your I2C bus.

[This PDF](https://github.com/jack-greenberg/SoftSysI2C2/blob/master/resources/Timers.pdf) is a great guide to understanding AVR timers. There are a lot of ways to configure timers to get them to operate at the correct frequency.

## Final Product

In the end, our code was able to transmit "Hello world" from an Arduino to be read by an Analog Discovery!

<figure align="center" width="50%"><img src="../documents/hello-world.png" /><figcaption>"Hello world" shown on an Analog Discovery</figcaption></figure>

The final code in `i2c.c` includes a `transmit_I2C` function that allows you to transmit a byte from a primary to a secondary device. There is also a framework for a `read_I2C` function that would allow you to read data from a secondary device, but we didn't have time to test it. 

The code that we have is meant to be portable to most AVR devices, and includes no Arduino macros or functions. We included a `Makefile` that can compile and flash Arduino from the terminal and also generates a `.a` file to be included in other embedded projects.

One issue with the code as it is right now is that it is a bit inconsistent, meaning that when you try to transmit "Hello world", you won't succeed 100% of the time. In fact, it is closer to 40-50% of the time. This is due to an issue with the way we set up our timer. We used a mode called CTC (explained more in <a href="#Design%20Decisions">Design Decisions</a>) which causes occasional lags in the I2C clock line throwing off the rest of the message. So sometimes you will end up transmitting "Hello w4$[SYN]" followed by an `ERR`. If we were to continue this project, we would probably look into using PWM (pulse width modulation) timer instead, which provides a more consistent frequency.

### Image Gallery

<div align="center">
    <figure>
        <img width="50%" src="../documents/schematic.png" />
    	<figcaption>Schematic of I2C Bus</figcaption>
    </figure>
    <br />
    <figure>
        <img width="50%" src="../data/scl.png" />
    	<figcaption>Fairly stable SCL (clock line) signal at 100kHz</figcaption>
    </figure>
    <br />
    <figure>
        <img width="50%" src="../data/address.png" />
    	<figcaption>Transmission of address over I2C (0110101)</figcaption>
    </figure>
</div>

## Design Decisions

### Timer

For the timer, we chose to use CTC (clear timer on compare) mode. This was in part because it was what we had the most experience with. CTC works by using the timer as a counter. You set a value to count up to and whenever counter hits that value, it resets. With a little bit of math you can set the value to give you a specific .frequency. We used the equation *counter_value* = *frequency<sub>CPU</sub>* / *desired_frequency* * 2. However, in reviewing the datasheet of the ATmega328p (the chip on the Arduino), it appears as though PWM (pulse width modulation) would have been a better solution as it might have provided a more stable signal. If we were to continue the project further, we would choose this option.

## Important Functions



## Reflection

In the end, this was a successful project. Our MVP was to have two Arduinos communicate with one another via I2C, and while we weren't able to do this because we didn't have the necessary equipment after departing Olin, we *were* able to get an Analog Discovery to read "Hello world" from an Arduino, so we consider that a success. It would have been nice to get a `read` function working as well, but it would have required a lot more time than we had.

### Jack's Reflection

I felt that I exceeded my goals. I feel much more comfortable now with embedded software, and I have developed an intuition about how to debug firmware. It has also deepened my understanding of C and how programs run at a very low level. I'm very glad I chose this project and took a lot away from it. I may continue working on it and develop it into a mature library with more students and open source it, especially because I haven't found any other examples of people doing an implementation of I2C *from scratch*. From what I've found, it has always been libraries that take advantage of I2C functionalities built into AVR chips, whereas our library just uses bit manipulation.

### David's Reflection






Content

Your project report should answer the following questions (note that some are the same as in the proposal and the update):

1) What is the goal of your project; for example, what do you plan to make, and what should it do?

2) What are your learning goals; that is, what do you intend to achieve by working on this project?

3) What resources did you find that were useful to you.  If you found any resources you think I should add to the list on the class web page, please email them to me.

4) What were you able to get done?  Include in the report whatever evidence is appropriate to demonstrate the outcome of the project.  Consider including snippets of code with explanation; do not paste in large chunks of unexplained code.  Consider including links to relevant files.  And do include output from the program(s) you wrote.

5) Explain at least one design decision you made.  Were there several ways to do something?  Which did you choose and why?

6) You will probably want to present a few code snippets that present the most important parts of your implementation.  You should not paste in large chunks of code or put them in the Appendix.  You can provide a link to a code file, but the report should stand alone; I should not have to read your code files to understand what you did.

7) Reflect on the outcome of the project in terms of your learning goals.  Between the lower and upper bounds you identified in the proposal, where did your project end up?  Did you achieve your learning goals?

Audience: Target an external audience that wants to know what you did and why.  More specifically, think about students in future versions of SoftSys who might want to work on a related project.  Also think about people who might look at your online portfolio to see what you know, what you can do, and how well you can communicate.

 You don't have to answer the questions above in exactly that order, but the logical flow of your report should make sense.  Do not paste the questions into your final report.

 

Submission Mechanics

1) In your project report, you should already have a folder called "reports" that contains a Markdown document called "update.md".  Make a copy of "update.md" called "report.md"

2) At the top of this document, give your report a title that at least suggests the topic of the project.  The title should not contain the name of the class or the words "project" or "report".

3) List the complete names of all members of the team. 

4) Answer the questions in the Content section, above. Use typesetting features to indicate the organization of the document.  Do not include the questions as part of your document.

5) Complete your update, view it on GitHub, and copy the GitHub URL.  Then paste the URL in the submission space below.  You only need one proposal for each team, but everyone should submit it.