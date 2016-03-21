;*****************************************************************************
; basic_delay.asm
;
; Assembly routine to perform a busy-loop delay in a loop.
;*****************************************************************************

;================================
; basic_delay subroutine
;
; synopsis:
;	Delay a fixed number of cycles in a busy loop.
;
;	The number of loops performed is passed in R16. The maximum
;	number of loops which can be performed is 256 (which is achieved 
;	by passing 0 in R16). Each iteration of the loop takes 3 clock
;	cycles. With a 16MHz clock, a clock cycle is 62.5 ns, so one
;	iteration of the loop is 187.5 ns. The maximum delay achievable
;	is (256 * 187.5ns) = 48 us.
;	
;	Note that the time spent in the delay is not exact because
;	time spent in interrupts is not accounted for. Of course you
;	can always disable global interrupts before invoking this method,
;	but do so at your own risk.
;
; inputs:
;	R16		- the number of loops to make
;
; Registers altered:
;	R16		- on exit, the register will be 0

basic_delay:
	dec	r16
	brne	basic_delay
	ret


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
