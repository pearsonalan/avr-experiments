;**********************************************
; test_delay.asm
;
; this progrm toggles PORTD4 every 1 ms
;**********************************************

.nolist
.include "../m328Pdef.asm"
.list

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

	; we want each time through this loop to be 1 ms or 16,000 cycles. 
	; in the basic_delay_l function each iteration is 4 cycles.
	; the instructions in this loop account for 12 cycles, so we ask
	; basic_delay to iterate for 3,997 iterations.
loop:
	eor	r18, r19		; toggle bit of r18 by performing exclusive or with 0x04
	out	PORTD, r18		; set the PORTD4 bit using the corresponding bit in r18

	ldi	r25, $0f		; load 3997 into r25:r24
	ldi	r24, $9d
	rcall	basic_delay_l		; delay for 3997 iterations

	rjmp	loop			; loop back to the start

.include "../basic-delay.asm"
