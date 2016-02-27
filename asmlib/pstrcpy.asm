;*****************************************************************************
; pstrcpy.asm
;
; Assembly routine to copy a null-terminated block of memory from program
; memory to SRAM
;
;*****************************************************************************

;================================
; pstrcpy subroutine
;
; synopsis:
;	Copy bytes from the buffer referenced by the Z register pair
;	to the buffer pointed to by Y register pair. The source buffer
;	is terminated by a 0 byte.
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
;
; Registers altered:
;	temp (R16)	- contains the last byte that was copied
;	Y (R29:R28)	- points at the end of destination buffer after the last copied byte
;	Z (R31:R30)	- points at the end of the source buffer after the last byte read

pstrcpy:
	lpm	temp, z+	; read byte into temp from Z pointer and increment pointer
	tst	temp		; see if the byte is 0
	breq	_pstrcpy_end	; jump to end if done
	st	y+, temp	; write byte to address in Y pointer and increment pointer
	rjmp	pstrcpy		; loop back
_pstrcpy_end:
	st	y, temp		; write the 0 byte to the destination, but do not increment
	ret

