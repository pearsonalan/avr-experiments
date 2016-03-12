;*****************************************************************************
; strrev.asm
;
; Assembly routine to reverse the bytes in a buffer
;*****************************************************************************

;================================
; strrev subroutine
;
; synopsis:
;	Reverse buffer pointed to by Y register pair. Length of buffer 
;	is passed in r25.
;
; inputs:
;	Y (R29:R28)	- pointer to buffer
;	R25		- count of bytes in buffer
;
; Registers altered:
;	Y (R29:R28)	- 
;	R25		- 
;	R16 (temp)	

strrev:
	tst	r25		; test if done
	breq	_strrev_end	; jump to end if done
_strrev_end:
	ret


