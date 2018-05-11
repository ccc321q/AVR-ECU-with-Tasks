#ifndef TABLE_VE_H
#define TABLE_VE_H

#include <inttypes.h>

#define VE_RPM_BINS 16
#define VE_MAP_BINS 16

extern uint16_t ve_rpm_table_ram[VE_RPM_BINS];
extern uint16_t ve_map_table_ram[VE_MAP_BINS];
extern uint8_t ve_table_ram[VE_MAP_BINS][VE_RPM_BINS];

void veTableLoadToRAM(void);
uint8_t veGet(uint16_t map, uint16_t rpm);
uint8_t veTableGet(uint8_t mapBin, uint8_t rpmBin);
uint16_t veTableRpmGet(uint8_t rpmBin);
uint16_t veTableMapGet(uint8_t mapBin);
void veTableSet(uint8_t mapBin, uint8_t rpmBin, uint8_t newValue);
void veTableRpmSet(uint8_t rpmBin, uint16_t newValue);
void veTableMapSet(uint8_t mapBin, uint16_t newValue);
uint8_t veTableGetWithOffset(uint8_t offset);
void veTableSetWithOffset(uint8_t offset, uint8_t newValue);

#endif
