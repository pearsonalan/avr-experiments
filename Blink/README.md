# Blink

The canonical _Hello World_ of Arduino.

This program will blink an LED attached to pin 13.

There is nothing particularly interesting about this implementation of Blink.  What is interesting
here is the GNU Makefiles to compile and upload the project to the Arduino.

## Circuit

```
Pin13 -> LED -> R220 -> GND
```

Connect Arduino Uno to USB port and wire circuit as above. 

## Install

Makefile works with GNU Make 3.81 on Ubuntu or Mac OS X. The location of the
Arduino IDE is specified in the architecture-specific __.mk__ files in
`../build/`.

```
$ make
$ make upload
```

## Install (Windows 10)

Makefile.win32 works on Windows 10 with Microsoft Visual Studio's `NMAKE` utility.
The Makefile.win32 expects Arduino IDE to be installed in `D:\Arduino`.

```
CMD> nmake /F Makefile.win32
CMD> nmake /F Makefile.win32 upload
```

## Notes

### Linux 
* Uses `/dev/ttyACM0` to talk to Arduino on Linux

### Mac OS X
* Uses `/dev/tty.usbmodem142401` to talk to Arduino on Mac OS X.

### Windows
* Uses `COM4` to talk to Arduino (use Device Manager > Ports to find COM port).
