% TFT UI

This describes the TFT object oriented UI I wrote.

This runs on the Adafruit 2.8" TFT display wired in 8-bit mode connected to
an Arduino Uno.

## Wiring

Wire the display as described in the
[Adafruit tutorial for 8bit display](https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/8-bit-wiring-and-test).

Wire the capacitive tocuch screen as described in the
[Adafruit tutorial](https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/capacitive-touchscreen).

* A5 -> SCL
* A4 -> SDA

## Libraries

Several Adafruit libraries must be installed to compile this:
* Adafruit_GFX
* Adafruit_TFTLCD
* Adafruit_BusIO
* Adafruit_FT6206

The Makefile is configured to run with the tools in the Arduino IDE v2.

I've been testing and developing this on Ramona (Mac OS X). I don't think the 
Makefiles work with Windows or Linux at the moment.

```
$ make
$ make upload
```

To see the serial output:

```
$ screen -port /dev/cu.usbmodem1411101 115200 
```

To update the device, you have to kill the serial connection with `^A - K`.
