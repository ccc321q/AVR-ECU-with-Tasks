#ifndef CONFIG_H
#define CONFIG_H

#include <inttypes.h>

void configVariablesSaveWithOffset(uint8_t byteOffset, uint8_t newByte);
uint8_t configVariablesGetWithOffset(uint16_t byteOffset);

struct config_t {
	uint16_t reqFuel;
	uint8_t engineStroke;
	uint8_t sparkTriggersPerCycle;
	uint8_t injSquirtsPerCycle;
	uint8_t injAlternateBank;
};

extern volatile struct config_t config;

#endif
