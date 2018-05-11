
#include "fuel_control.h"
#include "global.h"
#include "time.h"
#include "dispatch.h"
#include "config.h"
#include "helpers.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint16_t rpmTicks;

void fuelControlInit(void) {
	setTask(TSK_ON_SPARK_EVENT, &onSparkEvent);
	setTask(TSK_INJBANK1_OFF, &injBank1_Off);
	setTask(TSK_INJBANK2_OFF, &injBank2_Off);
	
	// Setup spark interrupt
	//EICRA = _BV(ISC00);				// INT0 spark interrupt: falling and rising edge generates interrupt
	// OR
	EICRA = _BV(ISC01);					// INT0 spark interrupt: only falling edge generates interrupt
	EIMSK = _BV(INT0);					// INT0 spark interrupt: enable spark interrupt
	
	// Setup Timer1 to measure time bewteen sparks for rpm calculation
	TCCR1A = 0;							// Normal mode
	TCCR1B |= (1<<CS10) | (1<<CS11);	// Start timer Normal mode, prescaller 64
}

void onSparkEvent(void) {
	static uint8_t sparkCount = 0;
	
	// update spark counter
	if (sparkCount >= config.sparkTriggersPerCycle) {
		sparkCount = 1;
	} else {
		sparkCount++;
	}
	
	// figure out when to measure RPM
	if (sparkCount == config.sparkTriggersPerCycle)
		calculateRpm();
		
	// figure out when to start injecting fuel
	if (sparkCount <= (config.sparkTriggersPerCycle / config.injSquirtsPerCycle)) {
		if (sparkCount == (config.sparkTriggersPerCycle / config.injSquirtsPerCycle)) {
			squirt();
		}
	} else if (sparkCount % (config.sparkTriggersPerCycle / config.injSquirtsPerCycle) == 0) {
		squirt();
	}
	
	unQueTask(TSK_ON_SPARK_EVENT);	// only run this task once per spark event
	
	
	
	
	/** TESTING **/
	queTaskDelayed(TSK_ON_SPARK_EVENT, rpmSimulationtTicks);	// this simulates rpm trigger
	/*************/
}

void squirt(void) {
	static uint8_t bank = 0;
	
	if (config.injAlternateBank) {
		
		if (bank == 0) {
			injBank1_On();
			queTaskDelayed(TSK_INJBANK1_OFF, (variables.injPulseWidth / config.injSquirtsPerCycle) * 2);
		} else {
			injBank2_On();
			queTaskDelayed(TSK_INJBANK2_OFF, (variables.injPulseWidth / config.injSquirtsPerCycle) * 2);
		}
		
		// toggle bank variable
		if (bank == 0)
			bank = 1;
		else
			bank = 0;
		
	} else {
		
		injBank1_On();
		queTaskDelayed(TSK_INJBANK1_OFF, (variables.injPulseWidth / config.injSquirtsPerCycle));
		injBank2_On();
		queTaskDelayed(TSK_INJBANK2_OFF, (variables.injPulseWidth / config.injSquirtsPerCycle));
	
	}
}

void calculateRpm(void) {
	/*** testing ***/
	rpmTicks = TCNT1;
	TCNT1 = 0;
	/***************/
	
	static uint16_t previousRpm = 0;
	
	// rpmTicks is incremented every 0.004ms / 4us
	if (rpmTicks == 0) {
		variables.rpm = 1;
	} else {
		uint32_t t = 15000000;
		variables.rpm = (uint16_t) (t / rpmTicks) * (config.engineStroke / 2);
		//variables.rpm = rpmTicks;
		variables.rpm = ((variables.rpm + previousRpm) >> 1);	// 2-sample moving average filter
	}
	previousRpm = variables.rpm;
	
	calculateInjPulseWidth(); // should this be called every squirt or every crank revolution like calculateRpm?
	
	/*
	static uint16_t previousRpm = 0;
	
	// rpmCounter is incremented in the 0.1ms ISR in time.c
	if (rpmCounter == 0) {
		variables.rpm = 0;
	} else {
		variables.rpm = (2400000 / rpmCounter) * (config.engineStroke / 2);
		variables.rpm = ((variables.rpm + previousRpm) >> 1);	// 2-sample moving average filter
	}
	previousRpm = variables.rpm;
	rpmCounter = 0;
	
	
	
	
	
	static uint32_t oldMicrosBetweenSparksX100 = 0;
	uint32_t microsBetweenSparksX100 = time.microsecondsX100 - oldMicrosBetweenSparksX100;
	if (microsBetweenSparksX100 == 0) {
		variables.rpm = 0;
	} else {
		variables.rpm = (600000 / microsBetweenSparksX100) * (config.engineStroke / 2);
	}
	oldMicrosBetweenSparksX100 = time.microsecondsX100;
	*/
}

void calculateInjPulseWidth(void) {
	// uint16_t means the range is 0 -> 6.55 seconds
	// uint8_t means range is 0 -> 25.5 milliseconds
	// 10 ticks in a millisecond
	
	// PW = REQ_FUEL * VE ( RPM , MAP) * MAP * γEnrich + γAccel + InjOpen
	// γEnrich = gamma_Enrich = (Warmup/100) * (O2_Closed Loop/100) * (AirCorr/100) * (BaroCorr/100)
	
	variables.injPulseWidth = config.reqFuel; // temporary
}

void injBank1_Off(void) {
	BIT_CLEAR(PORTB, 5); // led off
	BIT_CLEAR(PORTB, INJECTORS_BANK_1);
	
	unQueTask(TSK_INJBANK1_OFF);
}

void injBank1_On(void) {
	BIT_SET(PORTB, 5); // led on
	BIT_SET(PORTB, INJECTORS_BANK_1);
}

void injBank2_Off(void) {
	BIT_CLEAR(PORTB, INJECTORS_BANK_2);
	
	unQueTask(TSK_INJBANK2_OFF);
}

void injBank2_On(void) {
	BIT_SET(PORTB, INJECTORS_BANK_2);
}

/*** INTERRUPT: INT0 ignition *******************************************/
ISR(INT0_vect) {
	//rpmTicks = TCNT1;
	//TCNT1 = 0;
	//queTask(TSK_ON_SPARK_EVENT);
}

ISR(TIMER1_OVF_vect) {
	TCNT1 = 0;
}
