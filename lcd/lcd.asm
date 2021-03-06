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

	ldi	temp, (1<<DDD4)		; set the pin D4 to output
	out	DDRD, temp

	sei
	
	
;======================
; Main body of program:

main:
	rcall	lcd_init

	;rjmp	end

	ldi	r17, $00
	rcall	lcd_set_ddram_address
	ldi	r17, $48		; 'H'
	rcall	lcd_write_data

	ldi	r17, $01
	rcall	lcd_set_ddram_address
	ldi	r17, $65		; 'e'
	rcall	lcd_write_data

	;rjmp	end

	ldi	r17, $02
	rcall	lcd_set_ddram_address
	ldi	r17, $6c		; 'l'
	rcall	lcd_write_data

	ldi	r17, $6c		; 'l'
	rcall	lcd_write_data

	ldi	r17, $6f		; 'o'
	rcall	lcd_write_data

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
; Inputs:
;	R25:R24		- the number of loops to make
;
; Output:
;	none
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
;	none
;
; Outputs:
;	none
;
; Registers altered:
;	r1
;	r16
;	r17
;	r24
;	r25
;
lcd_init:
	ldi	temp, $7f		; set the DDRB register to have
	out	DDRB, temp		;   pins B0-B7 as output

	; wait 40ms for power to stabilize after power-up	
	rcall	delay_40_ms		; delay 40 ms
	
	; send init command, wait 4.1ms
	;   RS=0, RW=0, DB[7..4]=b0011
	ldi	temp, $03		; bits to set.
	out	PORTB, temp		;    set the pins by writing to IO PORT B
	rcall	lcd_pulse_e		; pulse the Enable flag
	rcall	delay_4_1_ms		; wait 4.1 ms

	; send init command, wait 100 us
	;   RS=0, RW=0, DB[7..4]=b0011
	ldi	temp, $03		; bits to set,
	out	PORTB, temp		;    set the pins by writing to IO PORT B
	rcall	lcd_pulse_e		; pulse the Enable flag
	rcall	delay_100_us		; wait 100 us

	; send init command, then wait for busy flag to go low
	ldi	temp, $03		; bits to set,
	out	PORTB, temp		;    set the pins by writing to IO PORT B
	rcall	lcd_pulse_e		; pulse the Enable flag
	rcall	delay_100_us		; wait 100 us

	rcall	pulse_d4			; pulse D4 pin after each step

	; Perform SET 4-BIT MODE.
	rcall	lcd_set_4bit
	
	rcall	pulse_d4			; pulse D4 pin after each step

	; Perform FUNCTION SET (Sets DL, N and F) values
	rcall	lcd_function_set

	rcall	pulse_d4			; pulse D4 pin after each step

	; Perform Function: SET DISPLAY ON
	rcall	lcd_set_display_on

	rcall	pulse_d4			; pulse D4 pin after each step

	; Perform Function: CLEAR DISPLAY
	rcall	lcd_clear_display

	rcall	pulse_d4			; pulse D4 pin after each step

	; Perform Function: ENTRY MODE SET
	rcall	lcd_entry_mode_set
	
	rcall	pulse_d4			; pulse D4 pin after each step

	ret


;=============================================================================
; lcd_set_4bit subroutine
;
; Synopsis:
;	Perform Function SET BIT MODE
;	Sets the folling values on the pins
;		RS=0, RW=0, DB[7..4]=$02
;	Then Toggles Enable, and waits for Busy to go low again
;
; Input:	none
;
; Ouptut:	none
;
; Registers altered: r1, r16, r24, r25
;
lcd_set_4bit:
	; Perform FunctionSet 4 BIT MODE:
	;   RS=0, RW=0, DB[7..4]=$02
	ldi	temp, $02		; put $02 in the input register
	out	PORTB, temp		;    set the pins by writing to IO PORT B
	rcall	lcd_pulse_e		; pulse the Enable flag
	jmp	lcd_wait_for_not_busy	; wait until the busy flag is off



;=============================================================================
; lcd_function_set subroutine
;
; Synopsis:
;	Perform FUNCTION SET
;	Sets the folling values on the pins:
;		DB7=0, DB6=0, DB5=1, DB4=DL, DB3=N DB2=F, DB1=0, DB0=0
;	DL is 0 to indicate 4 bit mode.
;	N and F are selected according to Table 8 on Page 29 of the datasheet.
;	For our 20x2 display, we use N=1, F=0
;	After each of the above commands, enable is toggled and then busy must go low
;
; Input:	none
;
; Ouptut:	none
;
; Registers altered: r1, r16, r17, r24, r25
;
lcd_function_set:
	; Perform Function SET DISPLAY MODE:
	;   RS=0, RW=0, DB[7..4]=00101000b
	ldi	r17, $28		; put $28 in the input register
	jmp	lcd_send_command	; send the command


;=============================================================================
; lcd_set_display_on subroutine
;
; Synopsis:
;
; Input:	none
;
; Ouptut:	none
;
; Registers altered: r1, r16, r17, r24, r25
;
lcd_set_display_on:
	; Perform Function SET DISPLAY CONTORL (D=ON, C=OFF, B=OFF):
	ldi	r17, $0C		; put $0C in the input register
	jmp	lcd_send_command	; send the command


;=============================================================================
; lcd_clear_display subroutine
;
; Synopsis:
;
; Input:	none
;
; Ouptut:	none
;
; Registers altered: r1, r16, r17, r24, r25
;
lcd_clear_display:
	; Perform Function CLEAR DISPLAY:
	ldi	r17, $01		; put $01 in the input register
	jmp	lcd_send_command	; send the command


;=============================================================================
; lcd_entry_mode_set subroutine
;
; Synopsis:
;
; Input:	none
;
; Ouptut:	none
;
; Registers altered: r1, r16, r17, r24, r25
;
lcd_entry_mode_set:
	; Perform Function ENTRY MODE SET:
	ldi	r17, $06		; put $06 in the input register
	jmp	lcd_send_command	; send the command


;=============================================================================
; lcd_set_ddram_address subroutine
;
; Synopsis:
;	Set the DDRAM address for the next read or write operation.  This also
;	sets up the LCD that the next read/write is going to be from display
;	data RAM (DDRAM).
;
; Input:
;	r17 - DDRAM address to set in lower 7 bits (high bit is unused)
;
; Registers altered:
;	r1
;	r16
;	r17	
;	r24
;	r25
;
lcd_set_ddram_address:
	; set the high bit of r17
	ori	r17, $80
	jmp	lcd_send_command
	

;=============================================================================
; lcd_send_command subroutine
;
; Synopsis:
;	Send an 8 bit command and wait for the BUSY FLAG to clear.
;	The high nibble is sent first, then the low nibble second.  Each
;	send of a nibble requires the EN flag to be toggled, then we can
;	wait for the not busy signal
;
; Inputs:
;	R17 - 8 bits command to send
;
; Outputs:
;	none
;
; Registers altered:
;	r1
;	r16
;	r24
;	r25
;
lcd_send_command:
	mov	r16, r17		; copy the 8 bits to send to r16
	swap	r16			; swap nibbles to get the high 4 bits into the low nibble
	rcall	lcd_send_command_nibble	; send the nibble containing the high 4 bits and pulse the Enable flag
	mov	r16, r17		; copy the bits to send to r16. now we send the low 4 bits
	rcall	lcd_send_command_nibble	; send the nibble containing the low 4 bits and pulse the Enable flag
	jmp	lcd_wait_for_not_busy	; wait until the busy flag is off
	


;=============================================================================
; lcd_send_command_nibble subroutine
;
; Synopsis:
;	Send half of an 8 bit command. The 4 bits to send are the low nibble
;	of R16. When sent in PORTB the low 4 bits correspond to DB[7..4]. The
;	upper nibble maps to the pins [-, RW, EN, RS], so set the upper nibble
;	to zeroes to enable writing a command
;
;	BIT:	7	6	5	4	3	2	1	0
;	VALUE:	-	RW	EN	RS	DB7	DB6	DB5	DB4
;	
; Inputs:
;	R16 - low 4 bits contain the bits to send
;
; Outputs:
;	none
;
; Registers altered:
;	r16
;	r24
;	r25
;
lcd_send_command_nibble:
	andi	r16, $0f		; clear high nibble.  This will also set RW, EN and RS
					;    low in addition to the 4 data pins to send
	out	PORTB, r16		; set the pins by writing to IO PORT B
	jmp	lcd_pulse_e		; pulse the Enable flag



;=============================================================================
; lcd_write_data
;
; Synopsis:
;	Write data to CGRAM or DDRAM depending on wheterh the last set address
;	command was the set CGRAM address command or set DDRAM address command
;	
; Inputs:
;	R17 - 8 bit value to write at the current address
;
; Outputs:
;	none
;
; Registers altered:
;	r1
;	r16
;	r17
;	r24
;	r25
;

lcd_write_data:
	mov	r16, r17		; copy the 8 bits to send to r16
	swap	r16			; swap nibbles to get the high 4 bits into the low nibble
	andi	r16, $0f		; clear high nibble.
	ori	r16, $10		; turn RS on (to set writing to RAM), turn RW off (to indicate writing)
	out	PORTB, r16		; set the pins by writing to IO PORT B
	rcall	lcd_pulse_e		; pulse the Enable flag

	mov	r16, r17		; copy the bits to send to r16. now we send the low 4 bits
	andi	r16, $0f		; clear high nibble.
	ori	r16, $10		; turn RS on (to set writing to RAM), turn RW off (to indicate writing)
	out	PORTB, r16		; set the pins by writing to IO PORT B
	rcall	lcd_pulse_e		; pulse the Enable flag

	jmp	lcd_wait_for_not_busy	; wait until the busy flag has cleared


;=============================================================================
; pulse_d4 subroutine
;
; Synopsis:
;	Quickly pulse the PIN on D4 high then low.
;
; Registers altered:
;	temp (r16)
;
pulse_d4:
	ldi	temp, $10
	out	PORTD, temp
	nop
	nop
	ldi	temp, $00
	out	PORTD, temp
	nop
	nop
	ret
	

;=============================================================================
; lcd_wait_for_not_busy subroutine
;
; Synopsis:
;	Poll the busy flag until we read that it is 0.
;
; Inputs:
;	none
;
; Output:
;	none
;
; Registers altered:
;	r1
;	r16 (temp)
;	r24
;	r25
;

lcd_wait_for_not_busy:
	jmp	delay_10_ms

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
;	none
;
; Output:
;	r1 is set to 1 or 0 based on the busy flag
;
; Registers altered:
;	r1
;	r16 (temp)
;	r24
;	r25
;
lcd_read_busy:
	ldi	temp, $70		; set the DDRB register to have
	out	DDRB, temp		;   pins B0-B3 as input, pins B4-B6 as output

	ldi	temp, $40		; set RW high to indicate that we are reading
	out	PORTB, temp		; set DB[7..4] low
					; set RS low

	rcall	lcd_pulse_e		; toggle the enable flag.  this method includes some wait time

	in	temp, PORTB		; now read the DB pins connected to PORTB into the temp reg
	clr	r1			; clear the r1 register
	bst	temp, 3			; copy bit 3 (the busy flag bit in DB[7]) into the T flag of the SREG
	bld	r1, 0			; copy the bit from the T flag into bit 0 of r1

	ldi	temp, $40
	out	PORTB, temp
	rcall	lcd_pulse_e		; toggle the enable flag.  this method includes some wait time

	ldi	temp, $7f		; set the DDRB register to have
	out	DDRB, temp		;   pins B0-B7 as output

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
;       Set Enable low
;
; Inputs:
;	none
;
; Outputs:
;	none
; 
; Registers altered:
;	r24
;	r25
;
lcd_pulse_e:
	cbi	PORTB, 5		; Set ENABLE low
	rcall	delay_500_ns		; Wait 500ns	
	sbi	PORTB, 5		; Set ENABLE high
	rcall	delay_500_ns		; Wait 500ns (LCD library comments says minimum 450ns)
	cbi	PORTB, 5		; Set ENABLE low
	rcall	delay_40_us		; Wait 40us to settle (LCD libary comment says >37us)	
	ret

