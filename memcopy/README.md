# memcopy

This directory contains experiments in how to implement memcopy 
in assembly language. Also of interest was how memcopy can
work in a Harvard architecture. Different instructions must be
used for reading string constants in program memory, so the 
same memcopy function cannot be used for copying progmem constants
as copying SRAM -> SRAM

The sim directory contains an AVR simulator that dumps the AVR
memory to a file when pin B0 is set high.  This allows us to
"see" the contents of the memory at different points of the execution
of the program.

To run the simulator, do:

```
make
cd sim
make
./avr-sim ../avr_memcpy.hex
```

