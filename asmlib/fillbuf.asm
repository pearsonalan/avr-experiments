;*****************************************************************************
; fillbuf.asm
;
; Assembly routine to fill a buffer of memory with consecutive integers
;*****************************************************************************

;================================
; fillbuf subroutine
;
; synopsis:
;	Fill the buffer pointed to by Y (r29:r28) with bytes from 0 .. r25
;
; inputs:
;	Y (R29:R28)	- pointer to buffer
;	R25		- count of bytes to fill
;
; Registers altered:
;	temp (R16)
;	Y (R29:R28)
;	R25

fillbuf:
	clr	temp			; clear temp register
_fb_loop:
	tst	r25			; test if done filling buffer
	breq	_fb_end			; jump to end if done
	st	y+, temp		; copy temp register into address pointed to by Y and increment pointer
	inc	temp			; increment temp register
	dec	r25			; decrememt counter register
	rjmp	_fb_loop		; loop back
_fb_end:
	ret

