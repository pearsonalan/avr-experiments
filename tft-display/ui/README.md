% TFT UI

This describes the TFT object oriented UI I wrote.

This runs on the Adafruit 2.8" TFT display wired in 8-bit mode connected to
an Arduino Uno.

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

