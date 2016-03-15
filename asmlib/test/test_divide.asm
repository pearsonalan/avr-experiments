;*****************************************************************************
; test_strcpy.asm
;
; Assembly program to test the operation of the strcpy implementation
;*****************************************************************************

.nolist
.include "../m328Pdef.asm"
.include "../regdefs.asm"
.list

.def	rem	=r15		;remainder
.def	res	=r16		;result
.def	dd	=r16		;dividend
.def	dv	=r17		;divisor
.def	cnt	=r18		;loop counter

;
; Set up the Interrupt Vector at 0x0000 to jump to the program start
;

.org 0x0000
	jmp reset		; PC = 0x0000	RESET

;======================
; initialization

.org 0x0034
reset:
	clr	r1			; set the SREG to 0
	out	SREG, r1
	ldi	r28, LOW(RAMEND)	; init the stack pointer
	ldi	r29, HIGH(RAMEND)
	out	SPL, r28
	out	SPH, r29
	
	
;======================
; Main body of program:

main:
	ldi	temp, (1<<DDB0)		; set the pin B0 to output
	out	DDRB, temp

	clr	temp			; set B0 low
	out	PORTB, temp
	nop

	ldi	dd, 74
	ldi	dv, 6
	call	div
	sts	$0100, res
	sts	$0108, rem

	ldi	dd, 100
	ldi	dv, 10
	call	div
	sts	$0110, res
	sts	$0118, rem

	call	toggle_b0		; toggle B0 to HIGH to trigger memory dump

	;; infinite loop at the end to keep the PC from falling off the end of the world
end:	rjmp	end			; do nothing forever


;======================
; toggle_b0 subroutine
;
; toggles pin B0 high then low as a signal to the simulator to dump the 
; contents of the memory.
;
; Registers altered: temp(r16)
;

toggle_b0:
	ldi	temp, (1<<PINB0)	; set B0 high
	out	PORTB, temp

	nop

	clr	temp			; set B0 low
	out	PORTB, temp

	nop
	ret


.include "../divide.asm"
