
#include "config.h"
#include "global.h"
#include <avr/eeprom.h>

// config_t structure is in global.h
volatile struct config_t config;		// data in RAM
struct config_t EEMEM config_eeprom;	// data in EEPROM


void configReadFromEeprom(void) {
	// read configuration variables from eeprom to ram
	eeprom_read_block((void*) &config, (const void*) &config_eeprom, sizeof(struct config_t));
}

void configSaveToEeprom(void) {
	eeprom_update_block((const void*) &config, (void*) &config_eeprom, sizeof(struct config_t));
}

void configVariablesSaveWithOffset(uint8_t offset, uint8_t newByte) {
	// set item in config structure
	if (offset < sizeof(struct config_t))
		*( (uint8_t *)&config + offset) = newByte;
	
	// save to eeprom when the last value is received
	if (offset == sizeof(struct config_t) - 1)
		configSaveToEeprom();
}

uint8_t configVariablesGetWithOffset(uint16_t offset) {
	// return item in config structure
	if (offset < sizeof(struct config_t))
		return *( (uint8_t *)&config + offset);
	else
		return 0;
}
