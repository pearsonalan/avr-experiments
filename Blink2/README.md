# Blink 2

This program will blink a pair of LEDs attached to pins 4 and 5.

## Circuit

```
Pin4 -> LED -> R220 -> GND
Pin5 -> LED -> R220 -> GND
```

Connect Arduino Uno to USB port and wire circuit as above. 

## Install

Makefile works on Ubuntu and expects Arduino IDE installed
in /usr/local/arduino-1.8.10.

```
make
make upload
```

## Notes

* Uses `/dev/ttyACM0` to talk to Arduino.
