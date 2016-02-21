;*****************************************************************************
; memcopy.asm
;
; Assembler routine to copy a block of memory
;*****************************************************************************

.nolist
.include "./m328Pdef.asm"
.list

.def temp = r16
.def overflows = r18

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
	
;======================
; Main body of program:

main:
	ldi	zl, LOW(message<<1)	; load address of message into zh:zl
	ldi	zh, HIGH(message<<1)	

end:
	nop
	rjmp	end			; do nothing forever

;======================
; Program data

.org 0x200
.cseg

message:
	.db	"Hello World", $0D, $0A, 0

