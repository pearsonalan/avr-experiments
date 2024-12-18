PANDOC=/usr/bin/pandoc

ifndef PORT
  PORT := /dev/ttyACM0
endif

ifndef BAUD
  BAUD := 115200
endif

ARDUINO_HOME=/usr/local/arduino-1.8.13
ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools
AVR_HOME=$(ARDUINO_HOME)/hardware/tools/avr

