ARCH=$(shell uname)

ifndef PORT
  PORT := /dev/tty.usbmodem142401
endif

ifndef BAUD
  BAUD := 115200
endif

ifeq ($(ARCH),Darwin)
  ARDUINO_HOME=/Applications/Arduino.app/Contents/Java
else ifeq ($(ARCH), Linux)
  ARDUINO_HOME=/usr/local/arduino-1.8.1
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

CFLAGS=-g -Os -Wall -Wextra -ffunction-sections -fdata-sections -MMD -mmcu=$(MCU)
CPPFLAGS=-g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=$(MCU)
DEFINES=-DF_CPU=16000000L -DARDUINO=10604 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
INCLUDES=-I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino \
	-I$(ARDUINO_HOME)/hardware/arduino/avr/variants/standard \
	-I$(ARDUINO_HOME)/libraries/Servo/src

OBJDIR=obj

CORE_OBJS= \
	$(OBJDIR)/wiring.o $(OBJDIR)/wiring_analog.o $(OBJDIR)/wiring_shift.o \
	$(OBJDIR)/wiring_pulse.o $(OBJDIR)/wiring_digital.o $(OBJDIR)/abi.o \
	$(OBJDIR)/hooks.o $(OBJDIR)/new.o $(OBJDIR)/Stream.o \
	$(OBJDIR)/Print.o $(OBJDIR)/CDC.o $(OBJDIR)/IPAddress.o $(OBJDIR)/USBCore.o \
	$(OBJDIR)/Tone.o $(OBJDIR)/WInterrupts.o $(OBJDIR)/WMath.o $(OBJDIR)/WString.o \
	$(OBJDIR)/HardwareSerial.o $(OBJDIR)/HardwareSerial0.o $(OBJDIR)/HardwareSerial1.o \
	$(OBJDIR)/HardwareSerial2.o $(OBJDIR)/HardwareSerial3.o $(OBJDIR)/main.o \
	$(OBJDIR)/Servo.o

CORE_LIB=$(OBJDIR)/ArduinoCore.a

PROG=rotary-encoder

OBJS=$(OBJDIR)/$(PROG).o
ELF=$(OBJDIR)/$(PROG).elf
EEP=$(OBJDIR)/$(PROG).eep
HEX=$(OBJDIR)/$(PROG).hex

PANDOC=/usr/local/bin/pandoc
PANDOC_TITLE_FLAG=
PANDOC_FLAGS=-t html -s --mathjax $(PANDOC_TITLE_FLAG)

.PHONY: all clean build upload

all: build docs

build: $(OBJDIR) $(ELF) $(EEP) $(HEX)

docs: README.html

$(OBJDIR):
	mkdir $(OBJDIR)

$(ELF): $(OBJS) $(CORE_LIB)
	$(CC) -Wall -Wextra -Os -Wl,--gc-sections -mmcu=$(MCU) -o $(ELF) $(OBJS) $(CORE_LIB) -lm 

$(EEP): $(ELF)
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(ELF) $(EEP)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $(ELF) $(HEX)

$(CORE_LIB): $(CORE_OBJS)
	$(AR) rcs $@ $^
	
clean:
	-rm $(OBJS)
	-rm $(CORE_OBJS) 
	-rm $(CORE_LIB)
	-rm $(ELF)
	-rm $(EEP)
	-rm $(HEX)
	-rm $(OBJDIR)/*.d
	-rm README.html

upload: $(EEP) $(HEX)
	$(AVRDUDE) -C$(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$(HEX):i 

$(PROG).o: $(PROG).cpp

$(OBJDIR)/%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(ARDUINO_SRC)/%.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(LIB_SERVO_SRC)/%.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(ARDUINO_SRC)/%.c
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.html: %.md
	$(PANDOC) $(PANDOC_FLAGS) -o $@ $<

