PANDOC=/usr/local/bin/pandoc

ifndef PORT
  PORT := /dev/tty.usbmodem142401
endif

ifndef BAUD
  BAUD := 115200
endif

ARDUINO_HOME=/Applications/Arduino.app/Contents/Java
ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools
AVR_HOME=$(ARDUINO_HOME)/hardware/tools/avr

