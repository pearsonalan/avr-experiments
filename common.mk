ARCH = $(shell uname)
COMMON_MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR := $(dir $(COMMON_MK_PATH))

# $(info ARCH is $(ARCH))
# $(info Path to common.mk = $(COMMON_MK_PATH))
# $(info Root dir = $(ROOT_DIR))

ifndef IDE
  IDE=1
endif

include $(ROOT_DIR)/build/$(ARCH).mk

$(info AVR_HOME is $(AVR_HOME))

CPP=$(AVR_HOME)/bin/avr-g++
CC=$(AVR_HOME)/bin/avr-gcc
AR=$(AVR_HOME)/bin/avr-ar
OBJCOPY=$(AVR_HOME)/bin/avr-objcopy
AVRSIZE=$(AVR_HOME)/bin/avr-size
AVRA=/usr/local/bin/avra

ifeq ($(IDE), 1)
	AVRDUDE=$(AVR_HOME)/bin/avrdude
	AVRDUDE_CONF=$(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf
else
	AVRDUDE=$(ARDUINO_TOOLS)/avrdude/6.3.0-arduino17/bin/avrdude
	AVRDUDE_CONF=$(ARDUINO_TOOLS)/avrdude/6.3.0-arduino17/etc/avrdude.conf
endif

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

ifeq ($(IDE), 1)
	ARDUINO_SRC=$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino
    VARIANTS=$(ARDUINO_HOME)/hardware/arduino/avr/variants
	SPI_SRC=$(ARDUINO_HOME)/hardware/arduino/avr/libraries/SPI/src
else
	ARDUINO_SRC=$(ARDUINO_HOME)/hardware/avr/1.8.6/cores/arduino
	VARIANTS=$(ARDUINO_HOME)/hardware/avr/1.8.6/variants
	WIRE_LIB=$(ARDUINO_HOME)/hardware/avr/1.8.6/libraries/Wire/src
	SPI_SRC=$(ARDUINO_HOME)/hardware/avr/1.8.6/libraries/SPI/src
endif

LIB_SERVO_SRC=$(ARDUINO_HOME)/libraries/Servo/src/avr

ADAFRUIT_BUSIO_LIB=$(ARDUINO_LIBRARIES)/Adafruit_BusIO

INCLUDES=-I$(ARDUINO_SRC) \
        -I$(VARIANTS)/standard \
        -I$(ARDUINO_HOME)/libraries/Servo/src \
		-I$(ADAFRUIT_BUSIO_LIB) \

ifeq ($(IDE), 2)
INCLUDES += -I$(WIRE_LIB)
endif

ifdef SPI
INCLUDES += -I$(SPI_SRC)
endif

ifdef ADAFRUIT_GFX
ADAFRUIT_GFX_LIB=$(ARDUINO_LIBRARIES)/Adafruit_GFX_Library
INCLUDES += -I$(ADAFRUIT_GFX_LIB)
endif

ifdef ADAFRUIT_TFTLCD
ADAFRUIT_TFTLCD_LIB=$(ARDUINO_LIBRARIES)/Adafruit_TFTLCD_Library
INCLUDES +=	-I$(ADAFRUIT_TFTLCD_LIB)
endif

OBJDIR=obj

CORE_OBJS= \
	$(OBJDIR)/wiring.o $(OBJDIR)/wiring_analog.o $(OBJDIR)/wiring_shift.o \
	$(OBJDIR)/wiring_pulse.o $(OBJDIR)/wiring_digital.o $(OBJDIR)/abi.o \
	$(OBJDIR)/hooks.o $(OBJDIR)/new.o $(OBJDIR)/Stream.o \
	$(OBJDIR)/Print.o $(OBJDIR)/CDC.o $(OBJDIR)/IPAddress.o $(OBJDIR)/USBCore.o \
	$(OBJDIR)/Tone.o $(OBJDIR)/WInterrupts.o $(OBJDIR)/WMath.o $(OBJDIR)/WString.o \
	$(OBJDIR)/HardwareSerial.o $(OBJDIR)/HardwareSerial0.o $(OBJDIR)/HardwareSerial1.o \
	$(OBJDIR)/HardwareSerial2.o $(OBJDIR)/HardwareSerial3.o

ifdef SERVO
CORE_OBJS += $(OBJDIR)/Servo.o
endif

ifdef SPI
CORE_OBJS += $(OBJDIR)/SPI.o
endif

ifdef ADAFRUIT_GFX
CORE_OBJS += $(OBJDIR)/Adafruit_GFX.o
endif

ifdef ADAFRUIT_TFTLCD
CORE_OBJS += $(OBJDIR)/Adafruit_TFTLCD.o
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
	$(AVRDUDE) -C$(AVRDUDE_CONF) -v -p$(AVRDUDE_MCU) -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$(HEX):i

upload-isp: $(EEP) $(HEX)
	$(AVRDUDE) -C$(AVRDUDE_CONF) -v -p$(AVRDUDE_MCU) -cusbtiny -Uflash:w:$(HEX):i

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

ifdef ADAFRUIT_GFX
$(OBJDIR)/%.o: $(ADAFRUIT_GFX_LIB)/%.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<
endif

ifdef ADAFRUIT_TFTLCD
$(OBJDIR)/%.o: $(ADAFRUIT_TFTLCD_LIB)/%.cpp
	$(CPP) -c $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<
endif

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

