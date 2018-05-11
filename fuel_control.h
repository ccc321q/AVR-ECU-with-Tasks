#ifndef FUEL_CONTROL_H
#define FUEL_CONTROL_H

#include <inttypes.h>

void fuelControlInit(void);
void onSparkEvent(void);
void calculateRpm(void);
void calculateInjPulseWidth(void);
void injBank1_Off(void);
void injBank1_On(void);
void injBank2_Off(void);
void injBank2_On(void);
void injSquirtsPerRev_Set(uint8_t newValue);
void squirt(void);

#endif
