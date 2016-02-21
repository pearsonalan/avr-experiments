## Notes on building/debugging AVR code using assembly

# Disassemble an ihex file

```
avr-objdump -m avr -D -S file.hex
```

# running in a simulator

Debug a .ihex file as so:

```
$ simavr -g -m atmega328p -f 16000000 serial-alpha.hex
```

Then in another window run
```
$ avr-gdb
```

In gdb, connect to the simulator:
```
(gdb) target remote localhost:1234
```

### Disassembling

To get a disassembly, you have to do some silly casting due to the Harvard architecture
of the ATmega chips:
```
(gdb) disas (void(*)()) 0x68, (void(*)()) 0x96
Dump of assembler code from 0x68 to 0x96:
=> 0x00000068:  rcall   .+4             ;  0x6e
   0x0000006a:  sei
   0x0000006c:  rjmp    .+40            ;  0x96
   0x0000006e:  ldi     r16, 0x67       ; 103
   0x00000070:  eor     r17, r17
   0x00000072:  sts     0x00C5, r17
   0x00000076:  sts     0x00C4, r16
   0x0000007a:  ldi     r16, 0x18       ; 24
   0x0000007c:  sts     0x00C1, r16
   0x00000080:  ldi     r16, 0x0E       ; 14
   0x00000082:  sts     0x00C2, r16
   0x00000086:  ret
   0x00000088:  lds     r16, 0x00C0
   0x0000008c:  sbrs    r16, 5
   0x0000008e:  rjmp    .-8             ;  0x88
   0x00000090:  sts     0x00C6, r19
   0x00000094:  ret
End of assembler dump.
```

Showing the disassembly:
```
set disassemble-next-line on
show disassemble-next-linek
```

Another cool way to do it is to use ASM layout:
```
(gdb) layout asm
```

### Running

 * Single-step instructions using `si`
 * Step over calls using `ni`

### Breakpoints

Setting a breakpoint requires a similar cast:
```
(gdb) b *(void(*)()) 0x8e
Breakpoint 5 at 0x8e
(gdb) cont
Continuing.
Breakpoint 4, 0x0000008e in ?? ()
```


