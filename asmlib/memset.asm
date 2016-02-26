;*****************************************************************************
; memset.asm
;
; Assembly routine to fill a block of memory with a single byte
;*****************************************************************************

;================================
; memset subroutine
;
; synopsis:
;	Set up to 255 bytes in the buffer pointed to by Y register pair to the value
;	in register r24. The count of bytes to set is passed in r25.
;
; inputs:
;	Y (R29:R28)	- pointer to destination buffer
;	R25		- count of bytes to fill
;	R24		- byte to fill the destination with
;
; Registers altered:
;	Y (R29:R28)	- points at the end of destination buffer after the last byte written
;	R25		- contains 0

memset:
	tst	r25		; test if done
	breq	_memset_end	; jump to end if done
	st	y+, r24		; write byte to address in Y pointer and increment pointer
	dec	r25		; decrement byte count register
	rjmp	memset		; loop back
_memset_end:
	ret

