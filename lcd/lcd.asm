;*****************************************************************************
; lcd.asm
;
; An of assembly program to interface with an Hitachi HD44780
; driven LCD. This may be made more general purpose in the future, but
; for now, it supports 2x20 displays.
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
; basic_delay_l subroutine
;
; synopsis:
;	Delay a fixed number of cycles in a busy loop.
;
;	The number of loops performed is passed in the register pair
;	R25:R24. The maximum number of loops which can be performed
;	is 65536 (which is achieved by passing $ff:$ff). Each iteration
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

; some fixed delays
delay_10ms:
	ldi	r25, $9c
	ldi	r24, $40
	rjmp	basic_delay_l

delay_40ms:
	clr	r25
	clr	r24
	rcall	basic_delay_l
	clr	r25
	clr	r24
	rcall	basic_delay_l
	ldi	r25, $1c
	ldi	r24, $40
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
	rcall	delay_40ms

	ret

