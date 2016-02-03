# WiiNunchuck

An experiment in talking to a Wii Nunchuck controller using the
arduino.

Initially, this project is simply the Arduino/Nunchuck interface
written by Chad Phillips and originally posted at
[http://www.windmeadow.com/node/42](https://web.archive.org/web/20140325144624/http://www.windmeadow.com/node/42).
I've taken the code he posted on his blog and updated it to work with
the Arduino 1.6.7 platform.  Also, since he mentioned that he needed
to make modifications to the included TWI library, I have included
the files for the TWI library here with the needed modifications.

## Compiling and installing

Compile:
```
make
```

Upload to Arduino:
```
make upload
```

If the Arduino is not connected to /dev/ttyACM0 as specified in the makefile,
the port may be overridden by an environment variable:
```
PORT=/dev/ttyUSB0 make upload
```

## Running the program

Wire the wii nunchuck as follows:

Red: +5V
White: Ground
Green: Analog Pin 4
Yellow: Analog Pin 5

View the serial output of the program by running
```
screen /dev/ttyACM0 19200 
```

