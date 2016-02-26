;*****************************************************************************
; memcopy.asm
;
; Assembler routine to copy a block of memory
;*****************************************************************************

.nolist
.include "../asmlib/m328Pdef.asm"
.include "../asmlib/regdefs.asm"
.list

.define BUFSZ 32


;
; Set up the Interrupt Vector at 0x0000
;
; We only use 1 interrupt in this program, the RESET
; interrupt.
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

	call	toggle_b0		; toggle B0 to HIGH to trigger memory dump

	ldi	r25, BUFSZ		; load the size of the source buffer into r25
	ldi	yl, LOW(src)		; load the address of the source buffer into yh:yl
	ldi	yh, HIGH(src)
	call	fillbuf			; invoke the fillbuf routine

	call	toggle_b0		; toggle B0 to HIGH to trigger memory dump

	ldi	r25, BUFSZ		; load the size of the source buffer into r25
	ldi	yl, LOW(dest)		; load the address of the destination buffer into yh:yl
	ldi	yh, HIGH(dest)
	ldi	zl, LOW(src)		; load the address of the source buffer into zh:zl
	ldi	zh, HIGH(src)
	call	memcopy			; invoke the memcopy routine

	call	toggle_b0		; toggle B0 to HIGH to trigger memory dump

	ldi	r25, 14			; load the size of the message into r25
	ldi	yl, LOW(dest)		; load the address of the destination buffer into yh:yl
	ldi	yh, HIGH(dest)
	ldi	zl, LOW(message<<1)	; load address of message into zh:zl
	ldi	zh, HIGH(message<<1)	
	call	pmemcopy		; invoke the pmemcopy routine

	call	toggle_b0		; toggle B0 to HIGH to trigger memory dump

	;;
	;; memsest some data
	;;

	ldi	yl, LOW(0x300)		; load the address where we are going to write hex values into yh:yl
	ldi	yh, HIGH(0x300)
	ldi	r25, 16
	ldi	r24, $de
	call	memset

	call	toggle_b0		; toggle B0 to HIGH to trigger memory dump

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


.include "../asmlib/fillbuf.asm"
.include "../asmlib/hexprint.asm"
.include "../asmlib/memcopy.asm"
.include "../asmlib/memset.asm"
.include "../asmlib/pmemcopy.asm"


;================================
; Program data
;

.cseg

message:
	.db	"Hello World", $0D, $0A, 0


;================================
; data segment
;

.dseg

;; buffer for memcopy source
src:	.byte	BUFSZ

;; buffer for memcopy destination
dest:	.byte	BUFSZ
	
