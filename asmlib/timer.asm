;*****************************************************************************
; timer.asm
;
; methods for timing stuff
;*****************************************************************************

.def overflows = r3

init_timer: 
	ldi	temp, 5			; set the Clock Selector Bits CS00, CS01, CS02 to 101
	out	TCCR0B, temp		; this puts Timer Counter0, TCNT0 in to FCPU/1024 mode
					; so it ticks at the CPU freq/1024

	ldi	temp, 0b00000001	; set the Timer Overflow Interrupt Enable (TOIE0) bit 
	sts	TIMSK0, temp		; of the Timer Interrupt Mask Register (TIMSK0)

	clr	temp
	out	TCNT0, temp		; initialize the Timer/Counter to 0
	ret

;====================
; simple delay function 
;   delay about 0.1 second

delay:
	clr	overflows		; set overflows to 0 
sec_count:
	mov	temp, overflows		; compare number of overflows and 6
	cpi	temp, 6
	brne	sec_count		; branch to back to sec_count if not equal 
	ret				; if 61 overflows have occured return

;=====================
; timer overflow interrupt handler
overflow_handler: 
	inc	overflows		; add 1 to the overflows variable
	reti				; return from interrupt
