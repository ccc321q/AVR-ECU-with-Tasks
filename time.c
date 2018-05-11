
#include "time.h"
#include "global.h"
#include "dispatch.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile struct time_t time;

void timeInit(void) {
	setTask(TSK_ON_TIME_TICK_EVENT, &onTimeTickEvent);
	
	// set all values in time structure to zero
	uint8_t i;
	for (i = 0; i < sizeof(struct time_t); i++)
		*((uint8_t *)&time + i) = 0;
	
	// Setup timer0 to interrupt at 10 kHz
	// this is the main tick period for the dispatcher
	TCCR0A = _BV(WGM01); 	// CTC mode
	TCCR0B = _BV(CS01);		// prescalar 8
	OCR0A = 199;			// timer counts, 199 = 10kHz
	TIMSK0 = _BV(OCIE0A);  	// enable output-compare interrupt TIMER0_COMPA
}

// this gets called everytime Timer0A interrupts at 10 kHz
void onTimeTickEvent(void) {
	// Tick the dispatcher
	dispatchTick();
	
	time.microsecondsX100 += 1;	// after 119 hours this will loop back to zero
	if ((time.microsecondsX100 % 10) == 0) {
		time.milliseconds += 1;
		if (time.milliseconds >= 1000) {
			time.milliseconds = 0;
			time.seconds += 1;
		}
	}
	unQueTask(TSK_ON_TIME_TICK_EVENT);
}

/*** INTERRUPT: Timer0A @ 0.1 ms / 100 microseconds ***************************************/
ISR(TIMER0_COMPA_vect) {
	queTask(TSK_ON_TIME_TICK_EVENT);
}
