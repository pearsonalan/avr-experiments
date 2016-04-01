;=============================================================================
; lcd.asm
;
; An of assembly program to interface with an Hitachi HD44780
; driven LCD. This may be made more general purpose in the future, but
; for now, it supports 2x20 displays.
;
; The code in this file implements the 4-bit data bus operation mode which
; requires fewer pins than the 8-bit data bus mode, but writes take longer.
;
; For reference, the Data Sheet for the HD44780U driver is at
;  https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
;
; The Device should be connected to the LCD as follows:
;	B0 => DB4
;	B1 => DB5
;	B2 => DB6
;	B3 => DB7
;	B4 => RS
;	B5 => ENABLE
;	B6 => READ/WRITE
;
;=============================================================================

;; Include the standard definitions for PORTS and REGISTERS
.nolist
.include "../asmlib/m328Pdef.asm"
.list


;=============================================================================
; Declarations

.def temp      = r16



;=============================================================================
; Interrupt Vector
;

.org 0x0000
	jmp reset				; Reset Handler
	jmp unhandled_interrupt			; IRQ0 Handler
	jmp unhandled_interrupt			; IRQ1 Handler
	jmp unhandled_interrupt			; PCINT0 Handler
	jmp unhandled_interrupt			; PCINT1 Handler
	jmp unhandled_interrupt			; PCINT2 Handler
	jmp unhandled_interrupt			; Watchdog Timer Handler
	jmp unhandled_interrupt			; Timer2 Compare A Handler
	jmp unhandled_interrupt			; Timer2 Compare B Handler
	jmp unhandled_interrupt			; Timer2 Overflow Handler
	jmp unhandled_interrupt			; Timer1 Capture Handler
	jmp unhandled_interrupt			; Timer1 Compare A Handler
	jmp unhandled_interrupt			; Timer1 Compare B Handler
	jmp unhandled_interrupt			; Timer1 Overflow Handler
	jmp unhandled_interrupt			; Timer0 Compare A Handler
	jmp unhandled_interrupt			; Timer0 Compare B Handler
	jmp unhandled_interrupt			; Timer0 Overflow Handler
	jmp unhandled_interrupt			; SPI Transfer Complete Handler
	jmp unhandled_interrupt			; USART, RX Complete Handler
	jmp unhandled_interrupt			; USART, UDR Empty Handler
	jmp unhandled_interrupt			; USART, TX Complete Handler
	jmp unhandled_interrupt			; ADC Conversion Complete Handler
	jmp unhandled_interrupt			; EEPROM Ready Handler
	jmp unhandled_interrupt			; Analog Comparator Handler
	jmp unhandled_interrupt			; 2-wire Serial Interface Handler
	jmp unhandled_interrupt			; Store Program Memory Ready Handler




;
; All Code should start after the Interrupt Vector, so the first real code 
; should be at address 0x0034
;

.org 0x0034



;=============================================================================
; unhandled_interrupt subroutine
; 
; Synopsis:
;	Unhandled Interrupt Vector routine. All unsed slots in the 
;	interrupt vector should point here, which simply does an iret, 
;	this implementing a nop routine
;

unhandled_interrupt:
	reti


;=============================================================================
; initialization

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
	rcall	lcd_init

	;; infinite loop at the end to keep the PC from falling off the end of the world
end:	rjmp	end			; do nothing forever



;================================
; basic_delay_l subroutine
;
; synopsis:
;	Delay a fixed number of cycles in a busy loop.
;
;	The number of loops performed is passed in the register pair
;	R25:R24. The maximum number of loops which can be performed
;	is 65536 (which is achieved by passing $00:$00). Each iteration
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

;=============================================================================
;
; some fixed delays.  These are pre-calculated numbers of loops to
; perform in the basic_delay_l routine. These routines will end up
; delaying a bit longer than the method names suggest because they 
; don't account for 2 LDI instructions, one RMP and one RET instruction.
; Thise means that the actual delays here are 8 clock cyles more than
; the requested time (or about 500ns too much.  To get this more exact
; the number of loops requested from basic_delay_l should be reduced
; by 2 in each case.
;    

; 500ns = 2 iterations (because of the reason above, this ends up being more like 1000ns)
delay_500_ns:
	ldi	r25, $00
	ldi	r24, $02
	rjmp	basic_delay_l

; 40us => 160 iterations ($00A0)
delay_40_us:
	ldi	r25, $0A
	ldi	r24, $00
	rjmp	basic_delay_l

; 100us => 400 iterations ($0190)
delay_100_us:
	ldi	r25, $01
	ldi	r24, $90
	rjmp	basic_delay_l

; 4.1ms => 16,400 iterations ($4010)
delay_4_1_ms:
	ldi	r25, $40
	ldi	r24, $10
	rjmp	basic_delay_l

; 10ms => 40,000 iterations (40,000 = $9C40)
delay_10_ms:
	ldi	r25, $9c
	ldi	r24, $40
	rjmp	basic_delay_l

; 40ms => 160,000 iterations => 65536 + 65536 + 28928
delay_40_ms:
	clr	r25
	clr	r24
	rcall	basic_delay_l
	clr	r25
	clr	r24
	rcall	basic_delay_l
	ldi	r25, $71		; 28928 => $7100
	ldi	r24, $00
	rjmp	basic_delay_l


;=============================================================================
; lcd_init subroutine
;
; Synopsis:
;	Perform initialization of the HD44780 LCD.  This implements the
;	Initialization By Instruction sequence for 4 bit operation mode
;	as described on Page 46 of the datasheet.
;
; Inputs:
;
; Registers altered:
;
lcd_init:
	ldi	temp, $7f		; set the DDRB register to have
	out	DDRB, temp		;   pins B0-B7 as output
	rcall	delay_40_ms		; delay 40 ms
	ldi	temp, $02		; set RW=>0, RS=>0, E=>0 DB[4..7]=>$02
	out	PORTB, temp		;    set the pins by writing to IO PORT B
	rcall	delay_4_1_ms		; wait 4.1 ms
	ldi	temp, $02		; write RW=>0, RS=>0, E=>0 DB[4..7]=>$02
	out	PORTB, temp		;    set the pins by writing to IO PORT B
	rcall	delay_100_us		; wait 100 us

	rcall	lcd_wait_for_not_busy	; wait until the busy flag is off

	ret


;=============================================================================
; lcd_wait_for_not_busy subroutine
;
; Synopsis:
;	Poll the busy flag until we read that it is 0.
;
; Registers altered:
;	temp (r16)
;	r1

lcd_wait_for_not_busy:
	rcall	lcd_read_busy		; read the busy flag into R1
	tst	r1			; see if r1 is 0
	brne	lcd_wait_for_not_busy	; if not 0, then LCD is busy, and loop
	ret


;=============================================================================
; lcd_read_busy subroutine
;
; Synopsis:
;	Read one byte from the HD44780 instruction register.  According to the
;	datasheet on page 9, when RW is 1 and RS is 0, after toggling Enable,
;	the busy flag can be read on DB[7], and the address counter can be read
;	on DB[0..6]. In the wiring for this circuit, only 4 pins are used so
;	we can't read DB[0..3].  So this routine only gets and returns the busy
;	flag on DB[7]
;
; Inputs:
;
; Output:
;	r1 is set to 1 or 0 based on the busy flag
;
; Registers altered:
;	temp (r16)
;	r1
;
lcd_read_busy:
	ldi	temp, $40		; set RW high to indicate that we are reading
	out	PORTB, temp		; set DB[4..7] low
					; set RS low

	rcall	lcd_pulse_e		; toggle the enable flag.  this method includes some wait time

	in	temp, PORTB		; now read the DB pins connected to PORTB into the temp reg
	clr	r1			; clear the r1 register
	bst	temp, 3			; copy bit 3 (the busy flag bit in DB[7]) into the T flag of the SREG
	bld	r1, 0			; copy the bit from the T flag into bit 0 of r1

	ldi	temp, $0F		; set RW low, RS low, ENABLE low, and DB[4..7] high
	out	PORTB, temp	

	ret


;============================================================================
; lcd_pulse_e subroutine
;
; Synopsis:
;	Pulse the ENABLE signal. The wriring has ENABLE connected to Pin B5.
;	The technique used in this algorithm is based on the
;	LiquidCrystal::pulseEnable function from the LiquidCrystal libary
;	that is distributed with Arduino.
;
;	Set ENABLE (PIN B5) high
;	Read the 4 data bus pins PORTB[0..3]
;   Set Enable low
;
lcd_pulse_e:
	cbi	PORTB, 5		; Set ENABLE low
	rcall	delay_500_ns		; Wait 500ns	
	sbi	PORTB, 5		; Set ENABLE high
	rcall	delay_500_ns		; Wait 500ns (LCD library comments says minimum 450ns)
	cbi	PORTB, 5		; Set ENABLE low
	rcall	delay_40_us		; Wait 40us to settle (LCD libary comment says >37us)	
	ret
