PROG := nunchuck

ifndef PORT
  PORT := /dev/ttyACM0
endif

ifndef BAUD
  BAUD := 115200
endif

ARDUINO_HOME=/usr/local/arduino-1.6.7
ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools

CPP=$(ARDUINO_TOOLS)/avr/bin/avr-g++
CC=$(ARDUINO_TOOLS)/avr/bin/avr-gcc
AR=$(ARDUINO_TOOLS)/avr/bin/avr-ar
OBJCOPY=$(ARDUINO_TOOLS)/avr/bin/avr-objcopy
AVRDUDE=$(ARDUINO_TOOLS)/avr/bin/avrdude

ARDUINO_SRC=$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino

CFLAGS=-c -g -Os -ffunction-sections -fdata-sections -MMD -mmcu=atmega328p
CPPFLAGS=-c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p
DEFINES=-DF_CPU=16000000L -DARDUINO=10604 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR -DTWI_FREQ=40000L
INCLUDES=-I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/standard

OBJDIR=obj

CORE_OBJS= \
	$(OBJDIR)/wiring.o $(OBJDIR)/wiring_analog.o $(OBJDIR)/wiring_shift.o \
	$(OBJDIR)/wiring_pulse.o $(OBJDIR)/wiring_digital.o $(OBJDIR)/abi.o \
	$(OBJDIR)/hooks.o $(OBJDIR)/new.o $(OBJDIR)/Stream.o  $(OBJDIR)/Print.o \
	$(OBJDIR)/WInterrupts.o $(OBJDIR)/WMath.o $(OBJDIR)/WString.o \
	$(OBJDIR)/HardwareSerial.o $(OBJDIR)/HardwareSerial0.o $(OBJDIR)/HardwareSerial1.o \
	$(OBJDIR)/HardwareSerial2.o $(OBJDIR)/HardwareSerial3.o \
	$(OBJDIR)/Wire.o $(OBJDIR)/twi.o $(OBJDIR)/main.o 

CORE_LIB=$(OBJDIR)/ArduinoCore.a

OBJS=$(OBJDIR)/Nunchuck.o
ELF=$(OBJDIR)/Nunchuck.elf
EEP=$(OBJDIR)/Nunchuck.eep
HEX=$(OBJDIR)/Nunchuck.hex

.PHONY: all clean build upload

all: build

build: $(OBJDIR) $(ELF) $(EEP) $(HEX)

$(OBJDIR):
	mkdir $(OBJDIR)

$(ELF): $(OBJS) $(CORE_LIB)
	$(CC) -Wall -Wextra -Os -Wl,--gc-sections -mmcu=atmega328p -o $(ELF) $(OBJS) $(CORE_LIB) -lm 

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
	-rmdir $(OBJDIR)

upload: $(EEP) $(HEX)
	$(AVRDUDE) -C$(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$(HEX):i 

$(PROG).o: Nunchuck.cpp

$(OBJDIR)/%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(ARDUINO_SRC)/%.cpp
	$(CPP) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJDIR)/%.o: $(ARDUINO_SRC)/%.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

