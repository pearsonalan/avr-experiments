;*****************************************************************************
; hexprint.asm
;
; Assembly routine to conver a digit to its hex representation in ascii
;*****************************************************************************

;================================
; hexprint subroutine
;
; synopsis:
;	Write the hex value of the contents of r25 to the 2 bytes pointed to
;	by the Y register pair.
;
; inputs:
;	Y (R29:R28)	- pointer to destination buffer
;	R25		- value to write
;
; Registers altered:
;	Y (R29:R28)	- points to the end of the destination buffer after the last byte written
;	temp (R16)	- contains the last byte that was copied
;	R17		- contains 'A'
;	R18		- contains '0'

hexprint:
	ldi	r17, $30	; put '0' in r17
	ldi	r18, ($41-$0A)	; put ('A'-10) in r18
	mov	temp, r25	; copy the whole byte value to temp
	andi	temp, $f0	; mask off the high nibble
	swap	temp		; swap the high nibble into the low nibble
	cpi	temp, 10	; compare the high nibble to 10
	brge	_hp_1		; if temp is less than 10
	add	temp, r17	;    add '0' to it to convert to a digit
	rjmp	_hp_2		; else
_hp_1:	add	temp, r18	;    add 'A' to it to conver to a hex letter
_hp_2:	st	y+, temp	; store the hex digit for the high nibble to the buffer
	mov	temp, r25	; copy the whole byte value to temp
	andi	temp, $0f	; mask off the low nibble
	cpi	temp, 10	; compare the low nibble to 10
	brge	_hp_3		; if temp is less than 10
	add	temp, r17	;    add '0' to it to convert to a digit
	rjmp	_hp_4		; else
_hp_3:	add	temp, r18	;    add 'A' to it to conver to a hex letter
_hp_4:	st	y+, temp	; store it to the buffer
	ret

