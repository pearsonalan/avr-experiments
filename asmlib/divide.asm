;
; AVR Division routines
;
; Taken from ATMEL application note 200
;

;***** Subroutine Register Variables

.def	rem	=r15		;remainder
.def	res	=r17		;result
.def	dd	=r17		;dividend
.def	dv	=r18		;divisor
.def	cnt	=r16		;loop counter

;***** Code

div8u:
	sub	rem, rem	;clear remainder and carry
	ldi	cnt, 9		;init loop counter
d8u_1:	rol	dd		;shift left dividend
	dec	cnt		;decrement counter
	brne	d8u_2		;if done
	ret			;    return
d8u_2:	rol	rem		;shift dividend into remainder
	sub	rem, dv		;remainder = remainder - divisor
	brcc	d8u_3		;if result negative
	add	rem, dv		;    restore remainder
	clc			;    clear carry to be shifted into result
	rjmp	d8u_1		;else
d8u_3:	sec			;    set carry to be shifted into result
	rjmp	d8u_1

