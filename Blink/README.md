# Blink

The canonical hello world of Arduino.

This program will blink an LED attached to pin 13.

## Circuit

```
Pin13 -> LED -> R220 -> GND
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
