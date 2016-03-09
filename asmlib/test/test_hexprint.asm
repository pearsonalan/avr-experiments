;*****************************************************************************
; test_strcpy.asm
;
; Assembly program to test the operation of the hexprint implementation
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

	;;
	;; print the hex values for $00 - $FF to RAM
	;;

	ldi	yl, LOW(0x400)		; load the address where we are going to write hex values into yh:yl
	ldi	yh, HIGH(0x400)
	clr	r25			; resest r25 to be $00
loop:
	call	hexprint		; print the current value of r25
	cpi	r25, $ff		; if we just printed $ff, it is done.
	breq	loop_end		;   - so get out of the loop
	inc	r25			; increment value to print
	rjmp	loop			; and loop to repeat
loop_end:
	call	toggle_b0		; toggle B0 to HIGH to trigger memory dump

	ldi	zl, LOW(0x400)		; load the address where we the data is into the zh:zl
	ldi	zh, HIGH(0x400)
	ldi	r25, 32			; the count of bytes to send over the serial out
	call	usart_transmit_buffer	; send the first 32 bytes over the usart
	
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


;
; Include Library routines
;

.include "../hexprint.asm"
.include "../timer.asm"
.include "../serial-blocking.asm"

