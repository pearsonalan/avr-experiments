ARCH=$(shell uname)

ifndef PORT
  PORT := /dev/ttyUSB0
endif

ifndef BAUD
  BAUD := 115200
endif

ifeq ($(ARCH),Darwin)
  ARDUINO_HOME=/Applications/Arduino.app/Contents/Java
else ifeq ($(ARCH), Linux)
  ARDUINO_HOME=/usr/local/arduino-1.6.7
else
  $(error unsupported architecture)
endif

ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools
AVR_HOME=$(ARDUINO_HOME)/hardware/tools/avr

ARDUINO_SRC=$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino
LIB_SERVO_SRC=$(ARDUINO_HOME)/libraries/Servo/src/avr

CPP=$(AVR_HOME)/bin/avr-g++
CC=$(AVR_HOME)/bin/avr-gcc
AR=$(AVR_HOME)/bin/avr-ar
OBJCOPY=$(AVR_HOME)/bin/avr-objcopy
AVRDUDE=$(AVR_HOME)/bin/avrdude
AVRSIZE=$(AVR_HOME)/bin/avr-size
AVRA=/usr/local/bin/avra

MCU=atmega328p

.PHONY: all upload sim

all: lcd.hex sim

lcd.hex: lcd.asm
	$(AVRA) -b lcd.o -l lcd.lst lcd.asm

sim:
	make -C sim

clean:
	-rm lcd.lst lcd.cof lcd.eep.hex lcd.hex lcd.o
	make -C sim clean

