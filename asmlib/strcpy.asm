;*****************************************************************************
; strcpy.asm
;
; Assembly routine to copy a block of memory terminated by a 0 byte
;
;*****************************************************************************

;================================
; strcpy subroutine
;
; synopsis:
;	Copy bytes from the buffer referenced by the Z register pair to the
;	buffer pointed to by Y register pair. The copy operation terminates
;	when a 0 byte is reached.
;
;	The source and destination buffers should not be overlapping.  The source
;	and destination buffers are also assumed to be in the data segment.
;
; inputs:
;	Y (R29:R28)	- pointer to destination buffer
;	Z (R31:R30)	- pointer to source buffer in data memory
;
; Registers altered:
;	temp (R16)	- contains the last byte that was copied
;	Y (R29:R28)	- points at the end of destination buffer after the last copied byte
;	Z (R31:R30)	- points at the end of the source buffer after the 0 byte

strcpy:
	ld	temp, z+	; read byte into temp from Z pointer and increment pointer
	tst	temp		; see if the byte is 0
	breq	_strcpy_end	; if 0, then we are done. jump to end
	st	y+, temp	; write byte to address in Y pointer and increment pointer
	rjmp	strcpy		; loop back
_strcpy_end:
	st	y, temp		; write the 0 byte to the destination, but do not increment
	ret


