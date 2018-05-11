
#ifndef ADC_H
#define ADC_H

#include <inttypes.h>

void adcInit(void);
void adcReadAllStart(void);
uint16_t adcReadChannel(uint8_t channel);

void calculateMap(void);
void calculateClt(void);
void calculateBatt(void);
void calculateEgo(void);
void calculateTps(void);
void calculateMat(void);

extern volatile uint16_t adc[];

#endif
