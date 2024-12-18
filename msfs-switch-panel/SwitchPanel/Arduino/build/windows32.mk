PANDOC=pandoc.exe

ifndef PORT
  PORT := COM4
endif

ifndef BAUD
  BAUD := 115200
endif

ARDUINO_HOME=D:/Arduino
ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools
AVR_HOME=$(ARDUINO_HOME)/hardware/tools/avr