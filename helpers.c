
#include "helpers.h"
#include "arithm.c"


/***************************************************************************/
/*** linear interpolation                                                ***/
/*** outMin <= outMax assumed		(inMin, inMax can be any)			 ***/
/*** Example:                                                            ***/
/*** engine.tps = linear_interp(config.tps_low, config.tps_high, 0, 255, sensors[TPS]); ***/
/***************************************************************************/
uint8_t linear_interpolate(uint8_t inMin, uint8_t inMax, uint8_t outMin, uint8_t outMax, uint8_t input) {
	uint8_t x21, dx;

	if (input <= inMin)  // below/equal lower bound
		return outMin;
	if (input >= inMax)  // above/equal upper bound
		return outMax;

	x21 = inMax - inMin;
	dx = input - inMin;

	// somehow gcc decides to do a signed division which we _don't_ want,
	// by splitting the expression, it can be tricked to do it unsigned.
	// *** someone should investigate why this happends ***

	if (outMax <= outMin) {  // negative slope
		uint16_t n;
		uint8_t r;
		
		n=(outMin-outMax)*dx;
		r = n / x21;
		return outMin - r;

	} else {        // positive slope
		uint16_t n;
		uint8_t r;
		
		n=(outMax-outMin)*dx;
		r = n / x21;
		return outMin + r;
	}
}

/***************************************************************************/
/*** mult div100                                                         ***/
/***************************************************************************/
uint16_t mult_div100(uint8_t a, uint16_t b) {
// NOTE that first parameter is 8 bit (correction factor around 100), 
// x*y/100 == appr.  x*y* 41 / 4096 (error in 0.1%)
	uint16_t t;
	uint32_t t32;	// let's be sure that the bitlength is what we think...
	t = 41*a;	// do not put into one expression with previous
	t32 = mult16_16(t, b);
	return div4096(t32);
}

// x*y/10 = appr. x*y* 205 / 2048 (error in 0.1%)
uint16_t mult_div10(uint8_t a, uint16_t b) {
	uint16_t t;
	uint32_t t32;	// let's be sure that the bitlength is what we think...
	t = 205*a;	// do not put into one expression with previous
	t32 = mult16_16(t, b);
	return t32 >> 11;
}

/* 
 * return a*b/10000 (a*b*41*41/4096/4096) (error in 0.2%)
 */
uint16_t mult_div10k(uint16_t a, uint16_t b) {
	uint32_t t32;
	uint16_t t16, t17;
	// always check the assembly output if you change a bit
	t16 = mult16_8(a, 41) >>8;
	t17 = mult16_8(b, 41) >>8;
	t32 = mult16_16(t16, t17);
	//t32 *= (uint32_t)(41*41);// unfortunately this is a long multiply, but somehow gcc overoptimizes (return 0) if I try to fit in 16 bit
	return t32>>8;
}

uint16_t mult_div25600(uint16_t a, uint16_t b) {
	uint32_t t32;
	uint16_t t16, t17;
	// always check the assembly output if you change a bit
	t16 = mult16_8(a, 131) >>8;
	t17 = mult16_8(b, 5) >>8;
	t32 = mult16_16(t16, t17);
	//t32 *= (uint32_t)(41*41);// unfortunately this is a long multiply, but somehow gcc overoptimizes (return 0) if I try to fit in 16 bit
	return t32>>8;
}
