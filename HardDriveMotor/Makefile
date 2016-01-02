PROG := HardDriveMotor

ifndef PORT
  PORT := /dev/ttyACM0
endif

ifndef BAUD
  BAUD := 115200
endif

ARDUINO_HOME=/usr/local/arduino-1.6.4
ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools

CPP=$(ARDUINO_TOOLS)/avr/bin/avr-g++
CC=$(ARDUINO_TOOLS)/avr/bin/avr-gcc
AR=$(ARDUINO_TOOLS)/avr/bin/avr-ar
OBJCOPY=$(ARDUINO_TOOLS)/avr/bin/avr-objcopy
AVRDUDE=$(ARDUINO_TOOLS)/avr/bin/avrdude

CFLAGS=-c -g -Os -Wall -Wextra -ffunction-sections -fdata-sections -MMD -mmcu=atmega328p
CPPFLAGS=-c -g -Os -Wall -Wextra -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p
DEFINES=-DF_CPU=16000000L -DARDUINO=10604 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
INCLUDES=-I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/standard

ARDUINO_SRC=$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino

CORE_OBJS= \
	wiring.o wiring_analog.o wiring_shift.o wiring_pulse.o wiring_digital.o \
	abi.o hooks.o new.o Stream.o HID.o Print.o CDC.o IPAddress.o USBCore.o Tone.o \
	WInterrupts.o WMath.o WString.o \
	HardwareSerial.o HardwareSerial0.o HardwareSerial1.o HardwareSerial2.o HardwareSerial3.o \
	main.o

CORE_LIB=core.a

OBJS=$(PROG).o
ELF=$(PROG).elf
EEP=$(PROG).eep
HEX=$(PROG).hex

.PHONY: all clean build upload

all: build

build: $(ELF) $(EEP) $(HEX)

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
	-rm *.d

upload: $(EEP) $(HEX)
	$(AVRDUDE) -C$(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$(HEX):i 

$(PROG).o: $(PROG).cpp

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.o: $(ARDUINO_SRC)/%.cpp
	$(CPP) $(CPPFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.o: $(ARDUINO_SRC)/%.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

