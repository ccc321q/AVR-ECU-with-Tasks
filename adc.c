
#include "adc.h"
#include "global.h"
#include "dispatch.h"
#include "helpers.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint8_t adcSelectedChannel;
volatile uint16_t adc[ADC_CHANNELS];

// initialize the sensor array
void adcInit(void) {
	uint8_t i;
	for (i=0; i<ADC_CHANNELS; i++) {
		adc[i] = adcReadChannel(i);
	}
	
	// start channel sampling
	setTask(TSK_ADC_READALL_START, &adcReadAllStart);
	queTask(TSK_ADC_READALL_START);
}

void adcReadAllStart(void) {
	// initilize mux, and set startbit
	adcSelectedChannel = 0;
	ADMUX = _BV(REFS0) | (adcSelectedChannel & 0x07);
	
	//#warning "what if the adc hasn't finished?"
	// note, a check is needed such that the adc isn't "overrun"
	ADCSRA = _BV(ADEN) |  _BV(ADSC) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	
	// re-run this function at a certain freq
	queTaskDelayed(TSK_ADC_READALL_START, ADC_SAMPLE_RATE);
}

uint16_t adcReadChannel(uint8_t channel) {

	/* init adc */
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);  // div-128 prescaler

	// left adjust result
	ADMUX = _BV(REFS0) | (channel & 0x0f);
	ADCSRA |= _BV(ADSC);

	loop_until_bit_is_set(ADCSRA, ADIF);
	ADCSRA |= _BV(ADIF); // clear adif-bit

	
	//return ADCH;	// ADCH contains the upper 8 bits of the 10-bit result
	return ADC;		// return all 10 bits
}


void calculateMap(void) {
	// using MPX4250AP 20 - 250 kPa MAP sensor
	// the same as variables.map = adc[ADC_MAP] * 0.25 + 12;
	variables.map = adc[ADC_MAP] * 25 / 100 + 12;
}

void calculateClt(void) {
	
}

void calculateBatt(void) {
	
}

void calculateEgo(void) {
	
}

void calculateTps(void) {
	
}

void calculateMat(void) {
	
}


/*** INTERRUPT: ADC conversion complete ********************************/
ISR(ADC_vect) {
	static uint16_t prev_sensor_reading[ADC_CHANNELS];
	uint16_t tmp;
	
	prev_sensor_reading[adcSelectedChannel] = adc[adcSelectedChannel];

	// ADCH contains the upper 8 bits of the 10-bit result
	//tmp = prev_sensor_reading[adcSelectedChannel] + ADCH;
	tmp = prev_sensor_reading[adcSelectedChannel] + ADC;
	adc[adcSelectedChannel] = tmp >> 1;	// 2-sample moving average filter
										// use a median filter instead, that would give nice readings

	adcSelectedChannel++;

	if (adcSelectedChannel >= ADC_CHANNELS) { // done
		adcSelectedChannel = 0;
		ADCSRA &= ~_BV(ADIE); // disable interrupt
	} else {
		ADMUX = _BV(REFS0) | (adcSelectedChannel & 0x07);
		ADCSRA |= _BV(ADSC); // start next conversion
	}
}
