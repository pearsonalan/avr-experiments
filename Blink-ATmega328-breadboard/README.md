% Blink

The canonical _Hello World_ of Arduino for the ATmega328 on a breadboard. Note that
this is _not_ the same as ATmega328p (the pico-power version which is used in
Arduino Uno).

This program will blink an LED attached to pin 13 of the ATmega328. Physical pin
13 is called **PIN_8** in the Arduino IDE (as defined in wiring.h, I believe).

## Circuit

```
Basic Power:
PIN1: Reset -> R10K -> GND
PIN7: Vcc
PIN8: GND
PIN20: Vcc
PIN21: Vcc
PIN22: GND

Programming (connection to AVR Pocket Programmer):
PIN1: Reset -> RST
PIN17: MOSI -> MOSI
PIN18: MISO -> MISO
PIN19: SCK -> SCK

LED:
Pin13 -> LED -> R220 -> GND

```

Place ATmega328 on breadboard and wire circuit as above. 
Connect AVR Pocket Programmer to Breadboard:

```

AVR Pocket Progammer Pin-out

  * MISO | VCC
    SCK  | MOSI
    RST  | GND

```

Note the lack of a crystal oscillator in this circuit, so we are using
the built-in 1MHz clock in this configuration.

## Install

Makefile works with GNU Make 3.81 on Ubuntu, Mac OS X or Windows.

The Makefile specifies `MCU=atmega328`

Since we are using AVR Pocket Programmer, we use `make upload-isp` rather than
`make upload`.

```
$ make
$ make upload-isp
```


## References

 * [https://www.allaboutcircuits.com/projects/breadboarding-and-programming-the-atmega328p-and-attiny45-in-atmel-studio-7/](https://www.allaboutcircuits.com/projects/breadboarding-and-programming-the-atmega328p-and-attiny45-in-atmel-studio-7/)
 * [https://www.allaboutcircuits.com/projects/atmega328p-fuse-bits-and-an-external-crystal-oscillator/](https://www.allaboutcircuits.com/projects/atmega328p-fuse-bits-and-an-external-crystal-oscillator/)

