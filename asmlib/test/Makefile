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
  ARDUINO_HOME=/usr/local/arduino-1.6.4
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

CFLAGS=-O0 -Wall -Wextra -ffunction-sections -fdata-sections -MMD -mmcu=$(MCU)
DEFINES=-DF_CPU=16000000L -DARDUINO=10604 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR

.PHONY: all upload_test_strcpy

HEXFILES = test_strcpy.hex test_serial_blocking.hex test_hexprint.hex test_strrev.hex test_divide.hex

EEPHEXFILES = $(addsuffix .eep.hex, $(basename $(HEXFILES)))
LSTFILES = $(addsuffix .lst, $(basename $(HEXFILES)))
COFFILES = $(addsuffix .cof, $(basename $(HEXFILES)))
OBJFILES = $(addsuffix .o, $(basename $(HEXFILES)))

all: $(HEXFILES)

upload_test_strcpy:
	avrdude -v -p m328p -c arduino -b $(BAUD) -P $(PORT) -U flash:w:test_strcpy.hex

clean:
	-rm $(HEXFILES) $(EEPHEXFILES) $(LSTFILES) $(COFFILES) $(OBJFILES)

%.hex: %.asm
	$(AVRA) -b $*.o -l $*.lst $<

test_strrev.hex: test_strrev.asm ../strrev.asm
test_strcpy.hex: test_strcpy.asm ../strcpy.asm
