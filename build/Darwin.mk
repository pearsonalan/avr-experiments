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

ifeq ($(IDE), 1) 
  $(info Configuring for Arduino IDE v.1)
  ARDUINO_HOME=/Applications/Arduino.app/Contents/Java
  ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools
  AVR_HOME=$(ARDUINO_HOME)/hardware/tools/avr
else
  $(info Configuring for Arduino IDE v.2)
  ARDUINO_HOME=$(HOME)/Library/Arduino15/packages/arduino
  ARDUINO_TOOLS=$(ARDUINO_HOME)/tools
  AVR_HOME=$(ARDUINO_TOOLS)/avr-gcc/7.3.0-atmel3.6.1-arduino7
  ARDUINO_LIBRARIES=$(HOME)/Documents/Arduino/libraries
endif
