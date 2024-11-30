ARCH = $(shell uname)
COMMON_MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR := $(dir $(COMMON_MK_PATH))

# $(info ARCH is $(ARCH))
# $(info Path to common.mk = $(COMMON_MK_PATH))
# $(info Root dir = $(ROOT_DIR))

include $(ROOT_DIR)/build/$(ARCH).mk

CPP=$(AVR_HOME)/bin/avr-g++
CC=$(AVR_HOME)/bin/avr-gcc
AR=$(AVR_HOME)/bin/avr-ar
OBJCOPY=$(AVR_HOME)/bin/avr-objcopy
AVRDUDE=$(AVR_HOME)/bin/avrdude
AVRSIZE=$(AVR_HOME)/bin/avr-size
AVRA=/usr/local/bin/avra

# Default to using the ATmega328P if no other MCU is specified
ifndef MCU
  MCU=atmega328p
endif

$(info MCU is $(MCU))

ifeq ($(MCU), atmega328p)
  $(info Building for ATmega328p)
  AVRDUDE_MCU=atmega328p
  ARDUINO_ID=10604
  ifndef F_CPU
    F_CPU=16000000L
  endif
  DEFINES := -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
endif

ifeq ($(MCU), atmega644pa)
  $(info Building for ATmega644pa)
  AVRDUDE_MCU=m644p
  ifndef F_CPU
    F_CPU=1000000L
  endif
endif

DEFINES := $(DEFINES) -DF_CPU=$(F_CPU)
ifdef ARDUINO_ID
  DEFINES := $(DEFINES) -DARDUINO=$(ARDUINO_ID) 
endif

CFLAGS=-g -Os -Wall -Wextra -ffunction-sections -fdata-sections -MMD -mmcu=$(MCU)
CPPFLAGS=-g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=$(MCU)

ARDUINO_SRC=$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino
LIB_SERVO_SRC=$(ARDUINO_HOME)/libraries/Servo/src/avr
SPI_SRC=$(ARDUINO_HOME)/hardware/arduino/avr/libraries/SPI/src

INCLUDES=-I$(ARDUINO_SRC) \
        -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/standard \
        -I$(ARDUINO_HOME)/libraries/Servo/src

ifdef SPI
INCLUDES += -I$(SPI_SRC)
endif

OBJDIR=obj

CORE_OBJS= \
	$(OBJDIR)/wiring.o $(OBJDIR)/wiring_analog.o $(OBJDIR)/wiring_shift.o \
	$(OBJDIR)/wiring_pulse.o $(OBJDIR)/wiring_digital.o $(OBJDIR)/abi.o \
	$(OBJDIR)/hooks.o $(OBJDIR)/new.o $(OBJDIR)/Stream.o \
	$(OBJDIR)/Print.o $(OBJDIR)/CDC.o $(OBJDIR)/IPAddress.o $(OBJDIR)/USBCore.o \
	$(OBJDIR)/Tone.o $(OBJDIR)/WInterrupts.o $(OBJDIR)/WMath.o $(OBJDIR)/WString.o \
	$(OBJDIR)/HardwareSerial.o $(OBJDIR)/HardwareSerial0.o $(OBJDIR)/HardwareSerial1.o \
	$(OBJDIR)/HardwareSerial2.o $(OBJDIR)/HardwareSerial3.o $(OBJDIR)/Servo.o

ifdef SPI
CORE_OBJS += $(OBJDIR)/SPI.o
endif

CORE_LIB=$(OBJDIR)/ArduinoCore.a

ARDUINO_MAIN_OBJS = $(OBJDIR)/main.o 
ARDUINO_MAIN_LIB=$(OBJDIR)/ArduinoMain.a


LIBS =

ifndef NO_CORE_LIB
LIBS += $(CORE_LIB)
endif

ifndef NO_ARDUINO_MAIN
LIBS += $(ARDUINO_MAIN_LIB)
endif

OBJS=$(OBJDIR)/$(PROG).o $(foreach module, $(MODULES), $(OBJDIR)/$(module).o)
ELF=$(OBJDIR)/$(PROG).elf
EEP=$(OBJDIR)/$(PROG).eep
HEX=$(OBJDIR)/$(PROG).hex

# PANDOC_TITLE_FLAG=--metadata title="Electronics Notebook - $(@B)"
PANDOC_TITLE_FLAG=
PANDOC_FLAGS=-t html -s --mathjax $(PANDOC_TITLE_FLAG)

.PHONY: all build clean docs dirs upload $(DIRS)

all: build docs dirs

build: $(OBJDIR) $(ELF) $(EEP) $(HEX)

$(OBJDIR):
	mkdir $(OBJDIR)

$(CORE_LIB): $(CORE_OBJS)
	$(AR) rcs $@ $^

$(ARDUINO_MAIN_LIB): $(ARDUINO_MAIN_OBJS)
	$(AR) rcs $@ $^

$(ELF): $(OBJS) $(LIBS)
	$(CC) -Wall -Wextra -Os -Wl,--gc-sections -mmcu=$(MCU) -o $(ELF) $(OBJS) $(LIBS) -lm 

$(EEP): $(ELF)
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(ELF) $(EEP)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $(ELF) $(HEX)

upload: $(EEP) $(HEX)
	$(AVRDUDE) -C$(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -v -p$(AVRDUDE_MCU) -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$(HEX):i

upload-isp: $(EEP) $(HEX)
	$(AVRDUDE) -C$(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -v -p$(AVRDUDE_MCU) -cusbtiny -Uflash:w:$(HEX):i

$(OBJDIR)/%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: %.cc
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(ARDUINO_SRC)/%.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(LIB_SERVO_SRC)/%.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(SPI_SRC)/%.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(ARDUINO_SRC)/%.c
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<


######################################
# Recurse into subdirectories

dirs: $(DIRS)

$(DIRS): 
	- $(MAKE) -C $@ $(MAKETARGET)


######################################
# Convert Markdown -> HTML in the current directory

docs: $(HTML)

%.html: %.md
	$(PANDOC) $(PANDOC_FLAGS) -o $@ $<


######################################
# Recusrsive clean

clean:
	-rm $(OBJS) $(CORE_OBJS) $(CORE_LIB) $(ARDUINO_MAIN_OBJS) $(ARDUINO_MAIN_LIB)
	-rm $(ELF) $(EEP) $(HEX) $(OBJDIR)/*.d $(HTML)

clean: clean-dirs

clean-dirs: MAKETARGET=clean
clean-dirs: $(DIRS)

