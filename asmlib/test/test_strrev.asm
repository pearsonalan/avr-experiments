;*****************************************************************************
; test_strrev.asm
;
; Assembly program to test the operation of the strrev implementation
;*****************************************************************************

.nolist
.include "../m328Pdef.asm"
.include "../regdefs.asm"
.list

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

	; write 9 bytes to $0300
	ldi	yl, $00
	ldi	yh, $03
	ldi	r25, 9
	call	fillbuf

	; write 16 bytes to $03f8 
	ldi	yl, $f8	
	ldi	yh, $03
	ldi	r25, 16
	call	fillbuf

	; write 1 byte to $0380
	ldi	r16, $80
	sts	$0380, r16

	; write 2 bytes to $0390
	ldi	r16, $90
	sts	$0390, r16
	inc	r16
	sts	$0391, r16

	; write 3 bytes to $03a0
	ldi	r16, $a0
	sts	$03a0, r16
	inc	r16
	sts	$03a1, r16
	inc	r16
	sts	$03a2, r16

	call	toggle_b0		; toggle B0 to HIGH to trigger memory dump

	ldi	yl, $00
	ldi	yh, $03
	ldi	r25, 9
	call	strrev

	ldi	yl, $f8			; load the address of the destination buffer into yh:yl
	ldi	yh, $03
	ldi	r25, 16
	call	strrev			; invoke the strcpy routine

	ldi	yl, $80			; load the address of the destination buffer into yh:yl
	ldi	yh, $03
	ldi	r25, 1
	call	strrev			; invoke the strcpy routine

	ldi	yl, $90			; load the address of the destination buffer into yh:yl
	ldi	yh, $03
	ldi	r25, 2
	call	strrev			; invoke the strcpy routine

	ldi	yl, $a0			; load the address of the destination buffer into yh:yl
	ldi	yh, $03
	ldi	r25, 3
	call	strrev			; invoke the strcpy routine

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


.include "../fillbuf.asm"
.include "../strrev.asm"

	
;================================
; Program data
;

.cseg

message:
	.db	"Hello World", $0D, $0A, 0

