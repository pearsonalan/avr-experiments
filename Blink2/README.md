# Blink 2

This program will blink a pair of LEDs attached to pins 4 and 5.

## Circuit

### Text Description

```
Pin4 -> LED -> R220 -> GND
Pin5 -> LED -> R220 -> GND
```

Connect Arduino Uno to USB port and wire circuit as above. 

### Schematic

![Schematic](images/Blink2_schem.svg)

![Breadboard](images/Blink2_bb.png)


## Install (Linux / Mac OS X)

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
