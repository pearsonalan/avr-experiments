PROG := blink

ifndef PORT
  PORT := /dev/ttyACM0
endif

ifndef BAUD
  BAUD := 115200
endif

ARDUINO_HOME=/usr/local/arduino-1.8.10
ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools

CC=$(ARDUINO_TOOLS)/avr/bin/avr-gcc
OBJCOPY=$(ARDUINO_TOOLS)/avr/bin/avr-objcopy
AVRDUDE=$(ARDUINO_TOOLS)/avr/bin/avrdude

CFLAGS=-Os -ffunction-sections -fdata-sections -MMD -mmcu=atmega328p
DEFINES=-DF_CPU=16000000L -DARDUINO=10604 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
INCLUDES=-I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/standard -I$(ARDUINO_HOME)/libraries/Servo/src

OBJDIR=obj

OBJS=$(OBJDIR)/$(PROG).o
#LIBS=-lm
LIBS=

ELF=$(OBJDIR)/$(PROG).elf
EEP=$(OBJDIR)/$(PROG).eep
HEX=$(OBJDIR)/$(PROG).hex

.PHONY: all clean build upload

all: build

build: $(OBJDIR) $(ELF) $(EEP) $(HEX)

$(OBJDIR):
	mkdir $(OBJDIR)

$(ELF): $(OBJS) $(CORE_LIB)
	$(CC) -Wall -Wextra -Os -Wl,--gc-sections -mmcu=atmega328p -o $(ELF) $(OBJS) $(LIBS)

$(EEP): $(ELF)
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(ELF) $(EEP)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $(ELF) $(HEX)

clean:
	-rm $(OBJS)
	-rm $(ELF)
	-rm $(EEP)
	-rm $(HEX)
	-rm $(OBJDIR)/*.d

upload: $(EEP) $(HEX)
	$(AVRDUDE) -C$(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$(HEX):i 

$(PROG).o: $(PROG).c

$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.s: %.c
	$(CC) -S $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<
