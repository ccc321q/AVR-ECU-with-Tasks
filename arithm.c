/*********************************************************\
*  AVR MegaSquirt - 2003                                  *
*  (C) 2003 [Marcell Gal cell at x-dsl.hu]                *
***********************************************************
* used for some simple arithmetics (included, not linked) *
\*********************************************************/

#ifndef __ARITHM_C__
#define __ARITHM_C__

static inline uint32_t mult16_16(uint16_t a, uint16_t b) {
	uint32_t result;
	uint8_t tmp;
/*
%A0 to refer to the lowest byte of the first operand, 
%A1 to the lowest byte of the second operand and so on...

%n0 : result
%n1 : tmp
%n2 : a
%n3 : b
*/

	asm (
		"\n"
		"clr %1 \n\t"
		"mul %B2, %B3 \n\t"  // hi(a) * hi(b)
		"movw %C0, r0 \n\t"
		"mul %A2, %A3 \n\t"  // lo(a) * lo(b)
		"movw %A0, r0 \n\t"
		"mul %B2, %A3 \n\t"  // hi(a) * lo(b)
		"add %B0, r0 \n\t"
		"adc %C0, r1 \n\t"
		"adc %D0, %1 \n\t"
		"mul %A2, %B3 \n\t"  // lo(a) * hi(b)
		"add %B0, r0 \n\t"
		"adc %C0, r1 \n\t"
		"adc %D0, %1 \n\t"
		"clr r1 \n\t"
		: "=&r" (result), "=&r" (tmp)
		: "r" (a), "r" (b)
	);

	return result;
}

static inline uint32_t mult16_8(uint16_t a, uint8_t b) {
	uint32_t result;

/*
%A0 to refer to the lowest byte of the first operand, 
%A1 to the lowest byte of the second operand and so on...

%n0 : result
%n1 : a
%n2 : b
*/

	asm (
		"\n"
		"clr %D0 \n\t"
		"clr %C0 \n\t"
		"mul %A1, %A2 \n\t"  // lo(a) * lo(b)
		"movw %A0, r0 \n\t"
		"mul %B1, %A2 \n\t"  // hi(a) * lo(b)
		"add %B0, r0 \n\t"
		"adc %C0, r1 \n\t"	// note that D0 stays 0
		"clr r1 \n\t"
		: "=&r" (result)
		: "r" (a), "r" (b)
	);

	return result;
}


/* just to avoid the 12*cycle (full 4byte each) gcc generates
*/
static inline uint16_t div4096(uint32_t a) {
	uint16_t result;

/*
%A0 to refer to the lowest byte of the first operand, 
%A1 to the lowest byte of the second operand and so on...

%n0 : result
%n1 : a
*/
	asm(
		"\n"
#if 1
		"swap %B1 \n\t"
		"swap %C1 \n\t"
		"swap %D1 \n\t"
		"ldi %A1, 0xf0 \n\t"

		"eor %D1, %C1 \n\t"
		"and %D1, %A1 \n\t"
		"eor %D1, %C1 \n\t"
				
		"eor %C1, %B1 \n\t"
		"and %C1, %A1 \n\t"
		"eor %C1, %B1 \n\t"
		"movw %A0, %C1 \n\t"
#else
		"movw %A0, %C1 \n\t"

		"lsl %B1 \n\t"
		"rol %A0 \n\t"
		"rol %B0 \n\t"

		"lsl %B1 \n\t"
		"rol %A0 \n\t"
		"rol %B0 \n\t"
				
		"lsl %B1 \n\t"
		"rol %A0 \n\t"
		"rol %B0 \n\t"
				
		"lsl %B1 \n\t"
		"rol %A0 \n\t"
		"rol %B0 \n\t"
#endif	
		: "=&r" (result)
		: "r" (a)
	);
	return result;
}

/* signed multiplication with offset trick 
a*b / 256
*/
static inline int8_t mult8i_8_div256(int8_t a, uint8_t b) {
	uint8_t t;
	t = (uint8_t)a + 0x80; // we offset it to avoid sign-problems
	t = (t * b) >> 8;
	return t - (b >> 1); // we revert the +0x80 sign-trick
}

/* the 0x80 could be a parameter, since it's sometimes used as 100 (eg.: adjust) */
static inline int16_t mult8i_8(int8_t a, uint8_t b) {
	uint8_t t;
	uint16_t t16;
	t = (uint8_t)a + 0x80; // we offset it to avoid sign-problems
	t16 = t * b;
	return t16 - (uint16_t)(b * 0x80); // we revert the +0x80 sign-trick
}

/* returns a * b / 256 */
static inline int16_t mult16i_8_div256(int16_t a, uint8_t b) {
	uint16_t t;
	t = (uint16_t)a + 0x8000; // we offset it to avoid sign-problems
	t = mult16_8(t, b) >> 8;
	return t - (uint16_t)(b * 0x80); // we revert the +0x80 sign-trick
}

/* signed downscale (divide) by 2th exponent */
static inline int16_t div16i_exp2(int16_t a, uint8_t exp) {
	/* for given exponent it could be tricked to something like (but not more efficient :-( ):
	if(exp == 4){
		uint16_t t;
		uint8_t sign;
		t = a;
		sign = t >> 8;
		t >>= exp; // this is unsigned, so GCC can use a swap
		if(sign & 0x80) // aha, it was negative!
			return t + 0xF000;

		return t;
	}
	*/
	// but generally GCC does a good job with (asr;ror) exp times
	return a >> exp;
}
#endif
