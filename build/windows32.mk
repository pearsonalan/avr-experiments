$(info processing windows32.mk)
PANDOC=pandoc.exe

ifndef ARDUINO_HOME
  $(info ARDUINO_HOME is not set)
endif

ifndef ARDUINO_HOME
  ifneq ("$(wildcard C:/Arduino)", "")
    $(info setting ARDUINO_HOME to C:/Arduino)
    ARDUINO_HOME := C:/Arduino
  else ifneq ("$(wildcard D:/Arduino)", "")
    $(info setting ARDUINO_HOME to D:/Arduino)
    ARDUINO_HOME := D:/Arduino
  else
    $(error ARDUINO_HOME could not be determined)
  endif
endif

ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools
AVR_HOME=$(ARDUINO_HOME)/hardware/tools/avr