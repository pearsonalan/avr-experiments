;***** Subroutine Register Variables

.def	rem	=r15		;remainder
.def	res	=r16		;result
.def	dd	=r16		;dividend
.def	dv	=r17		;divisor
.def	cnt	=r18		;loop counter

;***** Code

div:
	sub	rem, rem	;clear remainder and carry
	ldi	cnt,9		;init loop counter
d_1:	rol	dd		;shift left dividend
	dec	cnt		;decrement counter
	brne	d_2		;if done
	ret			;    return
d_2:	rol	rem		;shift dividend into remainder
	sub	rem, dv		;remainder = remainder - divisor
	brcc	d_3		;if result negative
	add	rem, dv		;    restore remainder
	clc			;    clear carry to be shifted into result
	rjmp	d_1		;else
d_3:	sec			;    set carry to be shifted into result
	rjmp	d_1

