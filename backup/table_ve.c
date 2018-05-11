
#include "table_ve.h"
#include "helpers.h"
#include <avr/eeprom.h>

// EEPROM
static uint16_t EEMEM ve_rpm_table[VE_RPM_BINS];
static uint16_t EEMEM ve_map_table[VE_MAP_BINS];
static uint8_t EEMEM ve_table[VE_MAP_BINS][VE_RPM_BINS];

// RAM
uint16_t ve_rpm_table_ram[VE_RPM_BINS];
uint16_t ve_map_table_ram[VE_MAP_BINS];
uint8_t ve_table_ram[VE_MAP_BINS][VE_RPM_BINS];

void veTableLoadToRAM(void) {
	int8_t mapBin, rpmBin;
	
	for (mapBin = 0; mapBin < VE_MAP_BINS; mapBin++) {
		for (rpmBin = 0; rpmBin < VE_RPM_BINS; rpmBin++) {
			ve_table_ram[mapBin][rpmBin] = veTableGet(mapBin, rpmBin);
		}
	}
	
	for (mapBin = 0; mapBin < VE_MAP_BINS; mapBin++) {
		ve_map_table_ram[mapBin] = veTableMapGet(mapBin);
	}
	
	for (rpmBin = 0; rpmBin < VE_RPM_BINS; rpmBin++) {
		ve_rpm_table_ram[rpmBin] = veTableRpmGet(rpmBin);
	}
}

uint8_t veGet(uint16_t map, uint16_t rpm) {
	uint8_t mapBinLower = VE_MAP_BINS - 1;
	uint8_t mapBinUpper = VE_MAP_BINS - 1;
	uint8_t rpmBinLower = VE_RPM_BINS - 1;
	uint8_t rpmBinUpper = VE_RPM_BINS - 1;
	
	// get upper and lower bins on the MAP axis
	uint8_t i;
	for (i = 0; i < VE_MAP_BINS; i++) {
		if (map < veTableMapGet(i)) {
			mapBinUpper = i;
			if ((i - 1) < 0)
				mapBinLower = 0;
			else
				mapBinLower = i - 1;
			break;
		}
	}
	
	// get upper and lower bins on the RPM axis
	for (i = 0; i < VE_RPM_BINS; i++) {
		if (map < veTableRpmGet(i)) {
			rpmBinUpper = i;
			if ((i - 1) < 0)
				rpmBinLower = 0;
			else
				rpmBinLower = i - 1;
			break;
		}
	}
	
	// TODO: BILINEAR INTERPOLATION HERE
	
	return ve_table_ram[mapBinLower][rpmBinLower];	// return value from RAM
}

void veTableSetWithOffset(uint8_t offset, uint8_t newValue) {
	uint8_t rpmBin, mapBin;
	
	if (offset >= (VE_RPM_BINS * VE_MAP_BINS)) {
		return;
	}
	
	if (offset < VE_RPM_BINS) {
		rpmBin = offset;
	} else {
		rpmBin = offset % VE_RPM_BINS;
	}
	
	mapBin = offset / VE_RPM_BINS;
	
	ve_table_ram[mapBin][rpmBin] = newValue;	// set RAM value
	veTableSet(mapBin, rpmBin, newValue);		// set EEPROM value
}

uint8_t veTableGetWithOffset(uint8_t offset) {
	uint8_t rpmBin, mapBin;
	
	if (offset >= (VE_RPM_BINS * VE_MAP_BINS)) {
		return 0;
	}
	
	if (offset < VE_RPM_BINS) {
		rpmBin = offset;
	} else {
		rpmBin = offset % VE_RPM_BINS;
	}
	
	mapBin = offset / VE_RPM_BINS;
	
	return ve_table_ram[mapBin][rpmBin];	// return value from RAM table
}

uint8_t veTableGet(uint8_t mapBin, uint8_t rpmBin) {
	return eeprom_read_byte(&ve_table[mapBin][rpmBin]);
}

uint16_t veTableRpmGet(uint8_t rpmBin) {
	return eeprom_read_word(&ve_rpm_table[rpmBin]);
}

uint16_t veTableMapGet(uint8_t mapBin) {
	return eeprom_read_word(&ve_map_table[mapBin]);
}

void veTableSet(uint8_t mapBin, uint8_t rpmBin, uint8_t newValue) {
	eeprom_update_byte(&ve_table[mapBin][rpmBin], newValue);	// set EEPROM value
	ve_table_ram[mapBin][rpmBin] = newValue;					// set RAM value
}

void veTableRpmSet(uint8_t rpmBin, uint16_t newValue) {
	eeprom_update_word(&ve_rpm_table[rpmBin], newValue);		// set EEPROM value
	ve_rpm_table_ram[rpmBin] = newValue;						// set RAM value
}

void veTableMapSet(uint8_t mapBin, uint16_t newValue) {
	eeprom_update_word(&ve_map_table[mapBin], newValue);		// set EEPROM value
	ve_map_table_ram[mapBin] = newValue;						// set RAM value
}
