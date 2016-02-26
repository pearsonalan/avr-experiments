;*****************************************************************************
; memcopy.asm
;
; Assembly routine to copy a block of memory
;*****************************************************************************

;================================
; memcopy subroutine
;
; synopsis:
;	Copy up to 255 bytes from the buffer referenced by the Z register pair
;	to the buffer pointed to by Y register pair. The count of bytes to
;	copy is passed in r25.
;
;	The source and destination buffers should not be overlapping.  The source
;	and destination buffers are also assumed to be in the data segment.
;
; inputs:
;	Y (R29:R28)	- pointer to destination buffer
;	Z (R31:R30)	- pointer to source buffer in data memory
;	R25		- count of bytes to fill
;
; Registers altered:
;	temp (R16)	- contains the last byte that was copied
;	Y (R29:R28)	- points at the end of destination buffer after the last copied byte
;	Z (R31:R30)	- points at the end of the source buffer after the last byte read
;	R25		- contains 0

memcopy:
	tst	r25		; test if done
	breq	_memcopy_end	; jump to end if done
	ld	temp, z+	; read byte into temp from Z pointer and increment pointer
	st	y+, temp	; write byte to address in Y pointer and increment pointer
	dec	r25		; decrement byte count register
	rjmp	memcopy		; loop back
_memcopy_end:
	ret

