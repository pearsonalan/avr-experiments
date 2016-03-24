;*****************************************************************************
; lcd.asm
;
; An of assembly program to interface with an Hitachi HD44780
; driven LCD. This may be made more general purpose in the future, but
; for now, it supports 2x20 displays.
;
; The Device should be connected to the LCD as follows:
;	B0 => DB4
;	B1 => DB5
;	B2 => DB6
;	B3 => DB7
;	B4 => RS
;	B5 => ENABLE
;	B6 => READ/WRITE
;
;*****************************************************************************

;; Include the standard definitions for PORTS and REGISTERS
.nolist
.include "../asmlib/m328Pdef.asm"
.list


;==============
; Declarations:

.def temp      = r16


;
; Set up the Interrupt Vector
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
	rcall	lcd_init

	;; infinite loop at the end to keep the PC from falling off the end of the world
end:	rjmp	end			; do nothing forever



;================================
; basic_delay_l subroutine
;
; synopsis:
;	Delay a fixed number of cycles in a busy loop.
;
;	The number of loops performed is passed in the register pair
;	R25:R24. The maximum number of loops which can be performed
;	is 65536 (which is achieved by passing $00:$00). Each iteration
;	of the loop takes 4 clock cycles. With a 16MHz clock, a clock
;	cycle is 62.5 ns, so one iteration of the loop is 250 ns. The
;	maximum delay achievable is (65536 * 250ns) = 16,384 us.
;
;	[A 1ms delay can be achieved by passing 4,000 (or $0F:$A0) in
;	the register pair]
;	
;	Note that the time spent in the delay is not exact because
;	time spent in interrupts is not accounted for. Of course you
;	can always disable global interrupts before invoking this method,
;	but do so at your own risk.
;
; inputs:
;	R25:R24		- the number of loops to make
;
; Registers altered:
;	R25		- on exit, the register will be 0
;	R24		- on exit, the register will be 0
;

basic_delay_l:
	sbiw	r25:r24, 1
	brne	basic_delay_l
	ret

;
; some fixed delays
;

; 10ms => 40,000 iterations (40,000 = $9C40)
delay_10_ms:
	ldi	r25, $9c
	ldi	r24, $40
	rjmp	basic_delay_l

; 4.1ms => 16,400 iterations ($4010)
delay_4_1_ms:
	ldi	r25, $40
	ldi	r24, $10
	rjmp	basic_delay_l

; 100us => 400 iterations ($0190)
delay_100_us:
	ldi	r25, $01
	ldi	r24, $90
	rjmp	basic_delay_l

; 40ms => 160,000 iterations => 65536 + 65536 + 28928
delay_40_ms:
	clr	r25
	clr	r24
	rcall	basic_delay_l
	clr	r25
	clr	r24
	rcall	basic_delay_l
	ldi	r25, $71		; 28928 => $7100
	ldi	r24, $00
	rjmp	basic_delay_l


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

	; delay 40 ms
	rcall	delay_40_ms

	; write RW=>0, RS=>0, E=>0 DB[4..7]=>$02
	ldi	temp, $02
	out	PORTB, temp

	; wait 4.1 ms
	rcall	delay_4_1_ms

	; write RW=>0, RS=>0, E=>0 DB[4..7]=>$02
	ldi	temp, $02
	out	PORTB, temp

	; wait 100 us
	rcall	delay_100_us

	ret


