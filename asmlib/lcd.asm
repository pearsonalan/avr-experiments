;*****************************************************************************
; lcd.asm
;
; A library of assembly routines to interface with an Hitachi HD44780
; driven LCD. This may be made more general purpose in the future, but
; for now, it supports 2x20 displays.
;
;
; The Device should be connected to the LCD as follows:
;	B0 => DATA0
;	B1 => DATA1
;	B2 => DATA2
;	B3 => DATA3
;	B4 => RS
;	B5 => ENABLE
;	B6 => READ/WRITE
;
;*****************************************************************************


;==============
; Declarations:

.def temp      = r16


;================================
; lcd_init subroutine
;
; synopsis
;
; inputs:
;
; Registers altered:
;
lcd_init:
	; set the DDRB register to have pins B0-B7 as output
	ldi	temp, $7f
	out	DDRB, temp
	ret

