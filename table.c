
#include "table.h"
#include "global.h"

struct table_t veTable;						// data in RAM
struct table_t EEMEM veTable_eeprom;		// data in EEPROM

struct table_t advanceTable;				// data in RAM
struct table_t EEMEM advanceTable_eeprom;	// data in EEPROM


/***** Universal table functions ************************************************/

void tableReadFromEeprom(struct table_t * tableRAM, struct table_t * tableEEPROM) {
	eeprom_read_block((void*) tableRAM, (const void*) tableEEPROM, sizeof(struct table_t));
}

void tableSaveToEeprom(struct table_t * tableRAM, struct table_t * tableEEPROM) {
	eeprom_update_block((const void*) tableRAM, (void*) tableEEPROM, sizeof(struct table_t));
}

void tableSaveYaxisWithOffset(struct table_t * tableRAM, struct table_t * tableEEPROM, uint8_t offsetL, uint8_t newValue) {
	uint16_t offset = offsetL + (TABLE_SIZE_X * TABLE_SIZE_Y * 2) + (TABLE_SIZE_X * 2);
	
	if (offset < (TABLE_SIZE_X * TABLE_SIZE_Y * 2) + (TABLE_SIZE_X * 2) + (TABLE_SIZE_Y * 2)) {		// *2 because the table is 16bit
		*((uint8_t *)&veTable + offset) = newValue;
		
		if (offset == (TABLE_SIZE_X * TABLE_SIZE_Y * 2) + (TABLE_SIZE_X * 2) + (TABLE_SIZE_Y * 2) - 1)
			tableSaveToEeprom(tableRAM, tableEEPROM);
	} else {
		return;
	}
}

void tableSaveXaxisWithOffset(struct table_t * tableRAM, struct table_t * tableEEPROM, uint8_t offsetL, uint8_t newValue) {
	uint16_t offset = offsetL + (TABLE_SIZE_X * TABLE_SIZE_Y * 2);
	
	if (offset < (TABLE_SIZE_X * TABLE_SIZE_Y * 2) + (TABLE_SIZE_X * 2)) {		// *2 because the table is 16bit
		*((uint8_t *)&veTable + offset) = newValue;
		
		if (offset == (TABLE_SIZE_X * TABLE_SIZE_Y * 2) + (TABLE_SIZE_X * 2) - 1)
			tableSaveToEeprom(tableRAM, tableEEPROM);
	} else {
		return;
	}
}

void tableSaveValuesWithOffset(struct table_t * tableRAM, struct table_t * tableEEPROM, uint8_t offsetU, uint8_t offsetL, uint8_t newValue) {
	uint16_t offset = MERGE_UINT8_TO_UINT16(offsetU, offsetL);
	
	if (offset < TABLE_SIZE_X * TABLE_SIZE_Y * 2) {		// *2 because the table is 16bit
		*((uint8_t *)&veTable + offset) = newValue;
		
		if (offset == TABLE_SIZE_X * TABLE_SIZE_Y * 2 - 1)
			tableSaveToEeprom(tableRAM, tableEEPROM);
	
	} else {
		return;
	}
}

int tableGetInterpolatedValue(struct table_t fromTable, int Y, int X){
	// X is usually RPM, Y is usually kPa
	
	//Loop through the X axis bins for the min/max pair
	//Note: For the X axis specifically, rather than looping from tableAxisX[0] up to tableAxisX[max], we start at tableAxisX[Max] and go down.
	// This is because the important tables (fuel and injection) will have the highest RPM at the top of the X axis, so starting there will mean the best case occurs when the RPM is highest (And hence the CPU is needed most)
	int xMinValue = fromTable.axisX[0];
	int xMaxValue = fromTable.axisX[TABLE_SIZE_X - 1];
	int xMin = 0;
	int xMax = 0;
	
	int x;
	for (x = TABLE_SIZE_X - 1; x > 0; x--)
	{
		//Checks the case where the X value is exactly what was requested
		if (X == fromTable.axisX[x-1])
		{
			xMaxValue = fromTable.axisX[x-1];
			xMinValue = fromTable.axisX[x-1];
			xMax = x-1;
			xMin = x-1;
			break;
		}
		//Normal case
		if ( (X <= fromTable.axisX[x]) && (X >= fromTable.axisX[x-1]) )
		{
			xMaxValue = fromTable.axisX[x];
			xMinValue = fromTable.axisX[x-1];
			xMax = x;
			xMin = x-1;
			break;
		}
	}
	
	//Loop through the Y axis bins for the min/max pair
	int yMinValue = fromTable.axisY[0];
	int yMaxValue = fromTable.axisY[TABLE_SIZE_Y - 1];
	int yMin = 0;
	int yMax = 0;
	
	int y;
	for (y = TABLE_SIZE_Y - 1; y > 0; y--)
	{
		//Checks the case where the Y value is exactly what was requested
		if (Y == fromTable.axisY[y-1])
		{
			yMaxValue = fromTable.axisY[y-1];
			yMinValue = fromTable.axisY[y-1];
			yMax = y-1;
			yMin = y-1;
			break;
		}
		//Normal case
		if ( (Y >= fromTable.axisY[y]) && (Y <= fromTable.axisY[y-1]) )
		{
			
			yMaxValue = fromTable.axisY[y];
			yMinValue = fromTable.axisY[y-1];
			yMax = y;
			yMin = y-1;
			break;
		}
	}
		
	
	/*
	At this point we have the 4 corners of the map where the interpolated value will fall in
	Eg: (yMin,xMin) (yMin,xMax)
	(yMax,xMin) (yMax,xMax)
	In the following calculation the table values are referred to by the following variables:
	A B
	C D
	*/
	int A = fromTable.values[yMin][xMin];
	int B = fromTable.values[yMin][xMax];
	int C = fromTable.values[yMax][xMin];
	int D = fromTable.values[yMax][xMax];
	
	//Create some normalised position values
	//These are essentially percentages (between 0 and 1) of where the desired value falls between the nearest bins on each axis
	/*
	// Float version
	float p = ((float)(X - xMinValue)) / (float)(xMaxValue - xMinValue);
	float q = ((float)(Y - yMaxValue)) / (float)(yMinValue - yMaxValue);
	float m = (1.0-p) * (1.0-q);
	float n = p * (1-q);
	float o = (1-p) * q;
	float r = p * q;
	return ( (A * m) + (B * n) + (C * o) + (D * r) );
	*/
	
	// Non-Float version:
	//Initial check incase the values were hit straight on
	int p;
	if (xMaxValue == xMinValue)
		{ p = ((X - xMinValue) << 7); } //This only occurs if the requested X value was equal to one of the X axis bins
	else
		{ p = ((X - xMinValue) << 7) / (xMaxValue - xMinValue); } //This is the standard case
	
	int q;
	if (yMaxValue == yMinValue)
		{ q = ((Y - yMinValue) << 7); }
	else
		{ q = ((Y - yMaxValue) << 7) / (yMinValue - yMaxValue); }
		
	int m = ((128-p) * (128-q)) >> 7;
	int n = (p * (128-q)) >> 7;
	int o = ((128-p) * q) >> 7;
	int r = (p * q) >> 7;

	return ( (A * m) + (B * n) + (C * o) + (D * r) ) >> 7; 
}


/******* VE table specific functions ***************************************/

// These can all be made universal table fuctions by adding the table to work on as a paramater.
// Unfortinately the whole uart rx cammand queue would have to be re written for that to work...

uint8_t veTableGetValuesWithOffset(uint16_t offset) {
	if (offset < TABLE_SIZE_X * TABLE_SIZE_Y * 2)	// *2 because the table is 16bit
		return *((uint8_t *)&veTable + offset);
	else
		return 0;
}

uint8_t veTableGetYaxisWithOffset(uint16_t offset) {
	offset += (TABLE_SIZE_X * TABLE_SIZE_Y * 2) + (TABLE_SIZE_X * 2);
	
	if (offset < (TABLE_SIZE_X * TABLE_SIZE_Y * 2) + (TABLE_SIZE_X * 2) + (TABLE_SIZE_Y * 2)) {		// * 2 because the table is 16bit
		return *((uint8_t *)&veTable + offset);
	} else {
		return 0;
	}
}

uint8_t veTableGetXaxisWithOffset(uint16_t offset) {
	offset += (TABLE_SIZE_X * TABLE_SIZE_Y * 2);
	
	if (offset < (TABLE_SIZE_X * TABLE_SIZE_Y * 2) + (TABLE_SIZE_X * 2)) {		// * 2 because the table is 16bit
		return *((uint8_t *)&veTable + offset);
	} else {
		return 0;
	}
}
