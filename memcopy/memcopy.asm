;*****************************************************************************
; memcopy.asm
;
; Assembler routine to copy a block of memory
;*****************************************************************************

.nolist
.include "./m328Pdef.asm"
.list

.define BUFSZ 32

.def temp = r16

;
; Set up the Interrupt Vector at 0x0000
;
; We only use 1 interrupt in this program, the RESET
; interrupt.
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
	ldi	temp, (1<<DDB0)		; set the pin B0 to output
	out	DDRB, temp

	clr	temp			; set B0 low
	out	PORTB, temp
	nop

	call	toggle_b0		; toggle B0 to HIGH, then LOW to store initial memory to dump

	ldi	r25, BUFSZ		; load the size of the source buffer into r25
	ldi	ZL, LOW(src<<1)		; load the address of the source buffer into ZH:ZL
	ldi	ZH, HIGH(src<<1)
	call	fill_buffer		; invoke the fill_buffer routine

	call	toggle_b0		; toggle B0 to HIGH, then LOW to store memory to dump after fill_buffer

	ldi	zl, LOW(message<<1)	; load address of message into zh:zl
	ldi	zh, HIGH(message<<1)	

end:
	nop
	rjmp	end			; do nothing forever



;======================
; toggle_b0 subroutine
;
; toggles pin B0 high then low as a signal to the simulator to dump the 
; contents of the memory.
;
; Registers altered: temp(r16)
;

toggle_b0:
	ldi	temp, (1<<PINB0)	; set B0 high
	out	PORTB, temp

	nop

	clr	temp			; set B0 low
	out	PORTB, temp

	nop
	ret


;======================
; fill_buffer subroutine
;
; fill the buffer pointed to by Z (r31:r30) with bytes
; from 0 .. r25

fill_buffer:
	clr	temp			; clear temp register
_fb_loop:
	tst	r25			; test if done filling buffer
	breq	_fb_end			; jump to end if done
	st	z+, temp		; copy temp register into address pointed to by Z and increment pointer
	inc	temp			; increment temp register
	dec	r25			; decrememt counter register
	rjmp	_fb_loop		; loop back
_fb_end:
	ret



;======================
; Program data
;

.cseg

message:
	.db	"Hello World", $0D, $0A, 0


;======================
; data segment
;

.dseg

;; buffer for memcopy source
src:	.byte	BUFSZ
	
