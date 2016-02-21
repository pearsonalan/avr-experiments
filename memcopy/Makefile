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

CFLAGS=-Os -Wall -Wextra -ffunction-sections -fdata-sections -MMD -mmcu=$(MCU)
DEFINES=-DF_CPU=16000000L -DARDUINO=10604 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR

.PHONY: all upload

all: memcopy.hex avr_memcpy.s avr_memcpy.hex simple_copy.s simple_copy.hex

simple_copy.hex: simple_copy.elf
simple_copy.elf: simple_copy.o
simple_copy.o: simple_copy.c

avr_memcpy.hex: avr_memcpy.elf
avr_memcpy.elf: avr_memcpy.o
avr_memcpy.o: avr_memcpy.c


memcopy.hex: memcopy.asm
	$(AVRA) -b memcopy.o -l memcopy.lst memcopy.asm

upload:
	avrdude -v -p m328p -c arduino -b $(BAUD) -P $(PORT) -U flash:w:memcopy.hex

clean:
	-rm memcopy.lst memcopy.cof memcopy.eep.hex memcopy.hex memcopy.o
	-rm simple_copy.hex simple_copy.elf simple_copy.o simple_copy.s simple_copy.d
	-rm avr_memcpy.hex avr_memcpy.elf avr_memcpy.o avr_memcpy.s avr_memcpy.d


%.elf: %.o
	$(CC) -Wall -Wextra -Os -Wl,--gc-sections -mmcu=atmega328p -o $@ $< -lm 

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	
%.o: %.c
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.s: %.c
	$(CC) -S $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

