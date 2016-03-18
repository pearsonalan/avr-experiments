;*****************************************************************************
; serial-blocking.asm
;
; blocking I/O methods for writing to the serial port through USART 0.
;*****************************************************************************

;================================================================
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


;================================================================
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



;================================================================
; usart_transmit_buffer subroutine
;
; synopsis:
;	Send up to 255 bytes from the buffer referenced by the Z register pair
;	to the USART.  The count of bytes to send is passed in r25
;
; inputs:
;	Z (R31:R30)	- pointer to source buffer in data memory
;	R25		- count of bytes to copy
;
; Registers altered and value on exit:
;	Z (R31:R30)	- points at the end of the source buffer after the last byte read
;	R19		- contains the last byte that was copied
;	R25		- contains 0
;

usart_transmit_buffer:
	tst	r25			; test if done sending bytes
	breq	_utbuf_end		; jump to end if done
	ld	r19, z+			; read byte into r19 from Z pointer and increment pointer
	rcall	usart_transmit_byte	; invoke usar_transmit_byte to send the value in R19
	dec	r25			; decrement byte count register
	rjmp	usart_transmit_buffer	; loop back
_utbuf_end:
	ret



;================================================================
; usart_transmit_pbuffer subroutine
;
; synopsis:
;	Send up to 255 bytes from the program memory buffer referenced by the Z register pair
;	to the USART.  The count of bytes to send is passed in r25
;
; inputs:
;	Z (R31:R30)	- pointer to source buffer in program memory
;	R25		- count of bytes to copy
;
; Registers altered and value on exit:
;	Z (R31:R30)	- points at the end of the source buffer after the last byte read
;	R19		- contains the last byte that was copied
;	R25		- contains 0
;

usart_transmit_pbuffer:
	tst	r25			; test if done sending bytes
	breq	_utpbuf_end		; jump to end if done
	lpm	r19, z+			; read byte into r19 from Z pointer and increment pointer
	rcall	usart_transmit_byte	; invoke usar_transmit_byte to send the value in R19
	dec	r25			; decrement byte count register
	rjmp	usart_transmit_pbuffer	; loop back
_utpbuf_end:
	ret

