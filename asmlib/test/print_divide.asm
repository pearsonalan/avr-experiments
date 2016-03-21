;*****************************************************************************
; print_divide.asm
;
; Assembly program to perform a division and print the result 
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

	rcall	usart_init_9600		; initialize the serial communications

	sei				; enable global interrupts


;======================
; Main body of program:

main:
	;; compute 144 / 6 (should be 24 R 0  => $18 R $00)
	ldi	r17, 144
	ldi	r18, 6
	rcall	divide_and_print

	;; compute 193 / 13 (should be 14 R 11  => $0E R $0B)
	ldi	r17, 193
	ldi	r18, 13
	rcall	divide_and_print

	;; compute 33 / 43 (should be 0 R 33  => $0 R $21)
	ldi	r17, 33
	ldi	r18, 43
	rcall	divide_and_print

	;; compute 10 / 1 (should be 10 R 0  => $0A R $00)
	ldi	r17, 10
	ldi	r18, 1
	rcall	divide_and_print

	;; compute 80 / 0 (should be ?)
	ldi	r17, 80
	ldi	r18, 0
	rcall	divide_and_print

end:	rjmp	end

;=======================
; divide_and_print subroutine
;
; synopsis:
;	Divide value in r16 by value in r17 and print the result
;       to the serial output
;
; inputs:
;	r17 - dividend
;	r18 - divisor
;
; Registers altered:
;	r16, r17, r18
;
divide_and_print:
	;; on entry r17 is the dividend r18 is the divisor
	rcall	div8u

	;; write the first part of the message to the USART
	ldi	zl, LOW(message1<<1)	; load the address of the data to print into zh:zl
	ldi	zh, HIGH(message1<<1) 
	ldi	r25, 15			; put the buffer length in r25
	rcall	usart_transmit_pbuffer	; invoke the usart_transmit_pbuffer routine to print it

	;; hex print the result in r17 to the 2 bytes of the printbuf
	ldi	yl, LOW(printbuf)	
	ldi	yh, HIGH(printbuf) 
	mov	r25, r17
	rcall	hexprint

	;; write the printbuf (2 bytes) to the USART
	ldi	zl, LOW(printbuf)	; load the address of the data to print into zh:zl
	ldi	zh, HIGH(printbuf) 
	ldi	r25, 2			; length of printbuf
	rcall	usart_transmit_buffer	; invoke the usart_transmit_pbuffer routine to print it

	;; write the second part of the message to the USART
	ldi	zl, LOW(message2<<1)	; load the address of the data to print into zh:zl
	ldi	zh, HIGH(message2<<1) 
	ldi	r25, 17			; put the buffer length in r25
	rcall	usart_transmit_pbuffer	; invoke the usart_transmit_pbuffer routine to print it

	;; hex print the result in r15 to the 2 bytes of the printbuf
	ldi	yl, LOW(printbuf)	
	ldi	yh, HIGH(printbuf) 
	mov	r25, r15
	rcall	hexprint

	;; write the printbuf (2 bytes) to the USART
	ldi	zl, LOW(printbuf)	; load the address of the data to print into zh:zl
	ldi	zh, HIGH(printbuf) 
	ldi	r25, 2			; length of printbuf
	rcall	usart_transmit_buffer	; invoke the usart_transmit_pbuffer routine to print it

	;; write the third part of the message to the USART
	ldi	zl, LOW(message3<<1)	; load the address of the data to print into zh:zl
	ldi	zh, HIGH(message3<<1) 
	ldi	r25, 2			; put the buffer length in r25
	rcall	usart_transmit_pbuffer	; invoke the usart_transmit_pbuffer routine to print it
	ret


.include "../serial-blocking.asm"
.include "../divide.asm"
.include "../hexprint.asm"
	

;================================
; Program data
;

.cseg

message1:
	.db	"The result is $" 
message2:
	.db	" with remainder $" 
message3:
	.db	$0D, $0A


;================================
; Data Segment
;

.dseg

printbuf:
	.byte	2
