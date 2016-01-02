.PHONY: all upload

all: clock-test.hex cycle-test.hex

clock-test.hex: clock-test.asm
	avra -l clock-test.lst clock-test.asm

cycle-test.hex: cycle-test.asm
	avra -l cycle-test.lst cycle-test.asm

upload-clock:
	avrdude -v -p m328p -c arduino -b 115200 -P /dev/ttyUSB0 -U flash:w:clock-test.hex
	
upload-clock-isp:
	avrdude -C /usr/local/arduino-1.6.4/hardware/tools/avr/etc/avrdude.conf -v -p atmega328p -c usbtiny -U flash:w:clock-test.hex
	
upload-cycle:
	avrdude -v -p m328p -c arduino -b 115200 -P /dev/ttyUSB0 -U flash:w:cycle-test.hex
	
upload-cycle-isp:
	avrdude -C /usr/local/arduino-1.6.4/hardware/tools/avr/etc/avrdude.conf -v -p atmega328p -c usbtiny -U flash:w:cycle-test.hex
	
clean:
	-rm clock-test.lst clock-test.cof clock-test.eep.hex clock-test.hex clock-test.obj
	-rm cycle-test.lst cycle-test.cof cycle-test.eep.hex cycle-test.hex cycle-test.obj
