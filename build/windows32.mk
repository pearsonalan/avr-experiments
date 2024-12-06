$(info processing windows32.mk)
PANDOC=pandoc.exe

ifndef ARDUINO_HOME
  $(info ARDUINO_HOME is not set)
endif

ifndef PORT
  PORT := COM3
endif

ifndef BAUD
  BAUD := 115200
endif

ifndef ARDUINO_HOME
  ifeq ($(IDE), 1)
    $(info Configuring build for Arduino V1)
    ifneq ("$(wildcard C:/Arduino)", "")
      ARDUINO_HOME := C:/Arduino
    else ifneq ("$(wildcard D:/Arduino)", "")
      ARDUINO_HOME := D:/Arduino
    else
      $(error ARDUINO_HOME could not be determined)
    endif
  else
    $(info Configuring build for Arduino V2)
    ARDUINO_HOME := $(LOCALAPPDATA)/Arduino15/packages/arduino
  endif
endif

$(info ARDUINO_HOME set to to $(ARDUINO_HOME))

ifeq ($(IDE), 1)
  ARDUINO_TOOLS=$(ARDUINO_HOME)/hardware/tools
  AVR_HOME=$(ARDUINO_HOME)/hardware/tools/avr
else
  ARDUINO_TOOLS=$(ARDUINO_HOME)/tools
  AVR_HOME=$(ARDUINO_TOOLS)/avr-gcc/7.3.0-atmel3.6.1-arduino7
endif
