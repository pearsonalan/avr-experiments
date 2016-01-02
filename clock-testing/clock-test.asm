;************************************
; clock-test.asm
;
; this progrm toggles PORTD4 every time
; a timer overflow occurs.
;************************************

.nolist
.include "./m328Pdef.inc"
.list

;==============
; Declarations:

.def temp      = r16
.def overflows = r17

;
; Set up the Interrupt Vector at 0x0000
;
; We only use 2 interrupts in this program, the RESET
; interrupt and the TIMER0 OVERFLOW interrupt.
;

.org 0x0000
	jmp reset		; PC = 0x0000	RESET

.org 0x0020
	jmp overflow_handler	; PC = 0x0020   TIMER0 OVF

;======================
; initialization

.org 0x0034
reset: 
	ldi	temp, 0b00000101
	out	TCCR0B, temp		; set the Clock Selector Bits CS00, CS01, CS02 to 101
					; this puts Timer Counter0, TCNT0 in to FCPU/1024 mode
					; so it ticks at the CPU freq/1024
	ldi	temp, 0b00000001
	sts	TIMSK0, temp		; set the Timer Overflow Interrupt Enable (TOIE0) bit 
					; of the Timer Interrupt Mask Register (TIMSK0)

	clr	r18
	ldi     r19, 0b00010000

	sei				; enable global interrupts -- equivalent to "sbi SREG, I"

	clr	temp
	out	TCNT0, temp		; initialize the Timer/Counter to 0

	sbi	DDRD, 4			; set PD4 to output


;======================
; Main body of program:

main:
	rjmp	main			; loop back to the start
  

; =====================
; timer overflow handler
overflow_handler: 
	eor	r18, r19		; toggle bit of r18 by performing exclusive or with 0x04
	out	PORTD, r18		; set the PORTD4 bit using the corresponding bit in r18
	reti				; return from interrupt

