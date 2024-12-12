% Blink with Serial communication

This program will blink an LED attached to pin 13.

## Circuit

This program was tested with the homebuilt "Breaduino" on a
perfboard that I found in my circuits cabinet.  I'm not sure
when or why I built this. It has:

* an ATmega329P microcontroller
* a 16Mhz oscillator
* An external power connector with switch
* A connector for the AVR Pocket Programmer
* A connector for the FT232R breakout

![Image of breaduino](img/IMG_0439.jpg)

## FT232 breakout

* [SparkFun product page](https://www.sparkfun.com/products/12731)
* [FTDI Tutorial at SparkFun](https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers)
* [FTDI drivers](https://ftdichip.com/drivers/vcp-drivers/)

![Image of FTDI breakout](img/12731-01.jpg)

Using the FT232 breakout required me to install a driver on Mac OS X.

On my Work Mac using Mac OS 14, it did not allow the driver to run.

## Install

Makefile works with GNU Make 3.81 on Ubuntu or Mac OS X. The location of the
Arduino IDE is specified in the architecture-specific __.mk__ files in
`../build/`.

Attach the AVR Pocket Programmer and upload the program using `make upload-isp`

![Image with AVR Programmer attached](img/IMG_0440.jpg)

```
$ make
$ make upload-isp
```

## Serial communications

The SparkFun FT232R breakout can be used for serial communications with this
board.

![Image with FT232R breakout attached](img/IMG_0441.jpg)

Then on Mac OS X, use screen to communicate with the device:

```
$ screen -port /dev/cu.usbmodem1411101 115200 
```

The device will likely be different than the path above. Look in the
`/dev` directory for a `cu.usbmodem*` device. The numbers seem to correspond
with which Hub it is connected.

You can't update the device through the USB port when screen is running. 
To update the device, you have to kill the serial connection with `^A - K`.


