#ifndef HELPERS_H
#define HELPERS_H

#include <inttypes.h>

uint8_t linear_interpolate(uint8_t inMin, uint8_t inMax, uint8_t outMin, uint8_t outMax, uint8_t input);
uint16_t mult_div100(uint8_t a, uint16_t b);
uint16_t mult_div10(uint8_t a, uint16_t b);
uint16_t mult_div10k(uint16_t a, uint16_t b);
uint16_t mult_div25600(uint16_t a, uint16_t b);

#endif
