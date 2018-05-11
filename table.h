#ifndef TABLE_H
#define TABLE_H

#include <inttypes.h>
#include <avr/eeprom.h>

//All tables are the same size for simplicity
#define  TABLE_SIZE_X 14
#define  TABLE_SIZE_Y 14

struct table_t {
	int values[TABLE_SIZE_Y][TABLE_SIZE_X];
	int axisX[TABLE_SIZE_X];
	int axisY[TABLE_SIZE_Y];
};

extern struct table_t veTable;
extern struct table_t EEMEM veTable_eeprom;

extern struct table_t advanceTable;
extern struct table_t EEMEM advanceTable_eeprom;


/** Universal table functions */
void tableReadFromEeprom(struct table_t * tableRAM, struct table_t * tableEEPROM);
void tableSaveToEeprom(struct table_t * tableRAM, struct table_t * tableEEPROM);
void tableSaveValuesWithOffset(struct table_t * tableRAM, struct table_t * tableEEPROM, uint8_t offsetU, uint8_t offsetL, uint8_t newValue);
void tableSaveYaxisWithOffset(struct table_t * tableRAM, struct table_t * tableEEPROM, uint8_t offsetL, uint8_t newValue);
void tableSaveXaxisWithOffset(struct table_t * tableRAM, struct table_t * tableEEPROM, uint8_t offsetL, uint8_t newValue);
int tableGetInterpolatedValue(struct table_t fromTable, int Y, int X);

/** VE table specific functions */
uint8_t veTableGetValuesWithOffset(uint16_t offset);
uint8_t veTableGetYaxisWithOffset(uint16_t offset);
uint8_t veTableGetXaxisWithOffset(uint16_t offset);

#endif
