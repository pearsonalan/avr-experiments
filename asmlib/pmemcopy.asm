;*****************************************************************************
; pmemcopy.asm
;
; Assembly routine to copy a block of memory from program memory to SRAM
;*****************************************************************************

;================================
; pmemcopy subroutine
;
; synopsis:
;	Copy up to 255 bytes from the buffer referenced by the Z register pair
;	to the buffer pointed to by Y register pair. The count of bytes to
;	copy is passed in r25.
;
;	The source buffer (Z) must reference an address in program memory.
;	The destination buffer (Y) must be in the data segment.
;
;	The subroutine is the same as the memcopy subroutine except that the
;	LPM instruction is used to read program memory instad of the LD
;	instruction which can only read data memory.
;
; inputs:
;	Y (R29:R28)	- pointer to destination buffer in data memory
;	Z (R31:R30)	- pointer to source buffer in program memory
;	R25		- count of bytes to copy
;
; Registers altered:
;	temp (R16)	- contains the last byte that was copied
;	Y (R29:R28)	- points at the end of destination buffer after the last copied byte
;	Z (R31:R30)	- points at the end of the source buffer after the last byte read
;	R25		- contains 0

pmemcopy:
	tst	r25		; test if done
	breq	_pmemcopy_end	; jump to end if done
	lpm	temp, z+	; read byte into temp from Z pointer and increment pointer
	st	y+, temp	; write byte to address in Y pointer and increment pointer
	dec	r25		; decrement byte count register
	rjmp	pmemcopy	; loop back
_pmemcopy_end:
	ret

