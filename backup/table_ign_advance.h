#ifndef TABLE_IGN_ADVANCE_H
#define TABLE_IGN_ADVANCE_H

#include <inttypes.h>

#define IGNITION_RPM_COUNT 16
#define IGNITION_MAP_COUNT 16

extern uint16_t ignition_rpm_table_ram[IGNITION_RPM_COUNT];
extern uint16_t ignition_map_table_ram[IGNITION_MAP_COUNT];
extern uint8_t ignition_table_ram[IGNITION_MAP_COUNT][IGNITION_RPM_COUNT];

void ignAdvTableLoadToRAM(void);
uint8_t ignTableGet(uint8_t mapBin, uint8_t rpmBin);
uint16_t ignTableRpmGet(uint8_t rpmBin);
uint16_t ignTableMapGet(uint8_t mapBin);

#endif
