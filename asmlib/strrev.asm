;*****************************************************************************
; strrev.asm
;
; Assembly routine to reverse the bytes in a buffer
;*****************************************************************************

;================================
; strrev subroutine
;
; synopsis:
;	Reverse buffer pointed to by Y register pair. Length of buffer 
;	is passed in r25.
;
; inputs:
;	Y (R29:R28)	- pointer to buffer
;	R25		- count of bytes in buffer
;
; Registers altered:
;	Y (R29:R28)	- 
;	Z (R31:R30)
;	R16
;	R17

;
; PSEUDOCODE:
; 
; 	ON START:
; 		Y = buffer start
; 		L = LEN
; 
; 		Z = Y + L  
; 	LOOP:
; 		IF Y >= Z GOTO END
; 		TMP = *Z
; 		*Z = *Y
; 		*Y = TMP
; 		Z = Z - 1
; 		Y = Y + 1
; 		GOTO LOOP
; 	END:
; 		RETURN

strrev:
	clr	r16

	; Z = Y + L 
	movw	zh:zl, yh:yl	; assign Z = Y
	add	zl, r25		; add LEN in r25 to Z 
	adc	zh, r16
	
_strrev_loop:
	; if Y >= Z GOTO END
	cp	yl, zl		; compare Y to Z
	cpc	yh, zh
	brsh	_strrev_end

	; swap value at (Z) with (Y) while decrementing
	; Z and incrementing Y
	ld	r17, -z
	ld	r16, y
	st	z, r16
	st	y+, r17
	
	; loop to the top
	rjmp	_strrev_loop
	

_strrev_end:
	
	ret


