;************************************
; cycle-test.asm
;
; this progrm toggles PORTD4 every clock cycle
;************************************

.nolist
.include "./m328Pdef.inc"
.list

;==============
; Declarations:

.def temp      = r16

;
; Set up the Interrupt Vector at 0x0000
;

.org 0x0000
	jmp reset		; PC = 0x0000	RESET


;======================
; Main body of program:

.org 0x0034
reset:
	sbi	DDRD, 4			; set PD4 to output
	clr	r18			; set r18 to 0 initially
	ldi     r19, 0b00010000		; set bit 5 to 1. this will toggle the PORTD4 bit

	jmp	simple_toggle

xor_toggle:
	eor	r18, r19		; toggle bit of r18 by performing exclusive or with 0x04
	out	PORTD, r18		; set the PORTD4 bit using the corresponding bit in r18
	rjmp	xor_toggle		; loop back to the start
  
simple_toggle:
	sbi	PORTD, 4		; set PORTD4 bit
	nop
	nop
	cbi	PORTD, 4		; clear PORTD4 bit
	rjmp	simple_toggle		; loop back to the start


