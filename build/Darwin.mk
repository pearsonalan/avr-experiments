PANDOC=/usr/local/bin/pandoc

# These PORT and BAUD settings are for serial upload using the SparkFun
# FTDI breakout to a "Boarduino" which is burned using the Duemilanove
# Bootloader
ifndef PORT
  PORT := /dev/cu.usbserial-AL00EZFL
endif

ifndef BAUD
  BAUD := 57600
endif

ARDUINO_HOME=/Applications/Arduino.app/Contents/Java
ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools
AVR_HOME=$(ARDUINO_HOME)/hardware/tools/avr

