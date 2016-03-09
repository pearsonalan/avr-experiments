;*****************************************************************************
; test_strcpy.asm
;
; Assembly program to test the operation of the blocking serial IO methods
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

;
; ADD the interrupt vector for TIMER0 overflow
.include "../timer-overflow-isr.asm"

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

	rcall	init_timer

	rcall	usart_init_9600		; initialize the serial communications

	sei				; enable global interrupts


;======================
; Main body of program:

main:
	ldi	zl, LOW(message<<1)	; load address of message into zh:zl
	ldi	zh, HIGH(message<<1)	

loop:
	lpm	r19, z+			; load the byte pointed at by Z and increment pointer
	tst	r19			; see if the byte is a 0 byte
	breq	newline			; branch if zero
	rcall	usart_transmit_byte	; send the character in r19 to the USART
	brne	loop			; if not go back and print again

newline:
	; send cr to the USART
	ldi	r19, $0D
	rcall	usart_transmit_byte

	; send newline to the USART
	ldi	r19, $0A
	rcall	usart_transmit_byte

	; wait for a bit
	rcall	delay
	
	rjmp	main			; else go back and to reset the Z pointer


.include "../timer.asm"
.include "../serial-blocking.asm"

.cseg

message:
	.db	"Hello World", 0

