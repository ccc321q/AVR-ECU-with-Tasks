#ifndef TIME_H
#define TIME_H

#include <inttypes.h>

struct time_t {
	volatile uint8_t seconds;
	volatile uint16_t milliseconds;
	volatile uint32_t microsecondsX100;
};

void timeInit(void);
void onTimeTickEvent(void);
extern volatile struct time_t time;

#endif
