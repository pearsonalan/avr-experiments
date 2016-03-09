;*****************************************************************************
; serial-blocking.asm
;
; blocking I/O methods for writing to the serial port through USART 0.
;*****************************************************************************

;================================
; usart_init_9600 subroutine
;
; synopsis:
;	Initialize the CPU to transmit on the USART0 at 9600 baud.
;
; inputs:
;
; Registers altered:
;	R16
;	R17
;

usart_init_9600:
	; these values are for 9600 Baud with a 16MHz clock
	ldi	r16, 103
	clr	r17

	; Set baud rate
	sts	UBRR0H, r17
	sts	UBRR0L, r16

	; Enable receiver and transmitter
	ldi	r16, (1<<RXEN0)|(1<<TXEN0)
	sts	UCSR0B, r16

	; Set frame format: Async, no parity, 8 data bits, 1 stop bit
	ldi	r16, 0b00001110
	sts	UCSR0C, r16
	ret


;================================
; usart_transmit_byte subroutine
;
; synopsis:
;	Send the byte in r19 over the serial wire.
;
; inputs:
;	R19		- byte to send
;
; Registers altered:
;	temp (R16)	- contains the last byte that was copied
;

usart_transmit_byte:
	; wait for empty transmit buffer
	lds	temp, UCSR0A
	sbrs	temp, UDRE0
	rjmp	usart_transmit_byte

	; Put data (r19) into buffer, sends the data
	sts	UDR0, r19
	ret



