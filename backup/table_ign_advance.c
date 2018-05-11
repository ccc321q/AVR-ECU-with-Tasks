
#include "table_ign_advance.h"
#include <avr/eeprom.h>

// EEPROM
static uint16_t EEMEM ignition_rpm_table[IGNITION_RPM_COUNT];
static uint16_t EEMEM ignition_map_table[IGNITION_MAP_COUNT];
static uint8_t EEMEM ignition_table[IGNITION_MAP_COUNT][IGNITION_RPM_COUNT];

// RAM
uint16_t ignition_rpm_table_ram[IGNITION_RPM_COUNT];
uint16_t ignition_map_table_ram[IGNITION_MAP_COUNT];
uint8_t ignition_table_ram[IGNITION_MAP_COUNT][IGNITION_RPM_COUNT];


void ignAdvTableLoadToRAM(void) {
	int mapBin, rpmBin;
	
	for (mapBin = 0; mapBin < IGNITION_MAP_COUNT; mapBin++) {
		for (rpmBin = 0; rpmBin < IGNITION_RPM_COUNT; rpmBin++) {
			ignition_table_ram[mapBin][rpmBin] = ignTableGet(mapBin, rpmBin);
		}
	}
	
	for (mapBin = 0; mapBin < IGNITION_MAP_COUNT; mapBin++) {
		ignition_map_table_ram[mapBin] = ignTableMapGet(mapBin);
	}
	
	for (rpmBin = 0; rpmBin < IGNITION_RPM_COUNT; rpmBin++) {
		ignition_rpm_table_ram[rpmBin] = ignTableRpmGet(rpmBin);
	}
}

uint8_t ignTableGet(uint8_t mapBin, uint8_t rpmBin) {
	return eeprom_read_byte(&ignition_table[mapBin][rpmBin]);
}

uint16_t ignTableRpmGet(uint8_t rpmBin) {
	return eeprom_read_word(&ignition_rpm_table[rpmBin]);
}

uint16_t ignTableMapGet(uint8_t mapBin) {
	return eeprom_read_word(&ignition_map_table[mapBin]);
}

// TODO ////////////////////////////////////////
