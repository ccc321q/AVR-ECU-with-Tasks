#ifndef GLOBAL_H
#define GLOBAL_H

#include <inttypes.h>

/*******  Done  *****************************

 * DONE all of dispatch works 100%
 * DONE simulate rpm by calling onSparkEvent at frequency
 * DONE last value of ve table is fucked, check algorithm
 * DONE squirts per engine cycle
 * DONE get RPM to work with simulated trigger
 * DONE make interface for reading / setting config variables - setting must store in eeprom
 * DONE make interface for reading / setting rpm and map axis on ve table

*********************************************/

/*******  TODO  *****************************

 * make tuner program
 * linear / bilinear interpolation when reading tables
 * convert adc to sensor values
 * check if alernate bank injection works - it should

 
*********************************************/

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

/* x=target variable, y=mask */
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) ((x) & (y))

#define MERGE_UINT8_TO_UINT16(x,y) ((uint16_t) ((x << 8) | (y & 0xFF)))

#define ADC_SAMPLE_RATE	50 	// 50 = 5ms with 10kHz dispatch tick
#define ADC_CHANNELS	6	// 6 is Max on ATmega328. THIS STARTS FROM 1, NOT 0.

/** Hardware definitions *****************************************/
//Port: C
#define ADC_MAP				0 // manifold absolute pressure
#define ADC_CLT				1 // coolant
#define ADC_BATT			2 // 12V voltage measurement
#define ADC_EGO 			3 // O2 sensor
#define ADC_TPS 			4 // throttle position sensor
#define ADC_MAT				5 // manifold air temperature

//Port: B
#define INJECTORS_BANK_1 	1 // injector port1	OSC1A
#define INJECTORS_BANK_2 	2 // injector port2	OSC1B
#define IDLEVALVE   		3 // idle solenoid	OSC2A
#define FUELPUMP    		4 // fuel pump
#define CLT_FAN     		5 // coolant fan

// Port: D
#define FLYBACK_BANK_1		3
#define FLYBACK_BANK_2		4

/*****************************************************************/

struct variables_t {
	uint8_t engineStatus;
	uint16_t rpm;
	uint8_t veCurrent;
	uint16_t injPulseWidth;
	uint16_t map;
	uint8_t clt;
	uint8_t batt;
	uint8_t ego;
	uint8_t tps;
	uint8_t mat;
	
	/*** used for testing ***/
	uint8_t veCurrentRpmBin;
	uint8_t veCurrentMapBin;
};

//engineStatus bits
#define RUNNING 		0  // engine running
#define CRANKING   		1  // cranking
#define WARMUP  		3  // warmup
#define ACCEL_ENRICH	6  // acceleration
#define IDLING   		7  // idle mode

/*****************************************************************/

// these are declared in main.c
extern volatile struct variables_t variables;
extern uint16_t rpmSimulationtTicks;

#endif
