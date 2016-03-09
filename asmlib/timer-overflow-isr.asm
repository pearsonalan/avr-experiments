;*****************************************************************************
; timer-overflow-isr.asm
;
; Assembly code to initialize the ISR vector for the timer overflow interupt
; on TIMER 0.
;*****************************************************************************

;
; Set up the Interrupt Vector:
;   0x0020 (TIMER0 OVF) => overflow_handler
;

.org 0x0020
	jmp overflow_handler	; PC = 0x0020   TIMER0 OVF


