/**
 * Author: Christiaan Aspeling
 * Email: christiaan.aspeling@hotmail.com
 * 2013
 * Auckland, New Zealand
 */

#include "main.h"
#include "fuel_control.h"
#include "adc.h"
#include "uart.h"
#include "global.h"
#include "time.h"
#include "table.h"
#include "dispatch.h"
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile struct variables_t variables;		// variables_t structure is in global.h

/*** TESTING ***/
uint16_t rpmSimulationtTicks = 400; // 92 = 13000 rpm, 171 = 7000rpm, 400 = 3000rpm, 4800 = 250rpm
/***************/

void mainInit(void) {
	// Port B
	DDRB |= (1 << INJECTORS_BANK_1) | (1 << INJECTORS_BANK_2) | (1 << IDLEVALVE) | (1 << FUELPUMP) | (1 << CLT_FAN);	// set pins as output
	PORTB &= ~((1 << INJECTORS_BANK_1) | (1 << INJECTORS_BANK_2) | (1 << IDLEVALVE) | (1 << FUELPUMP) | (1 << CLT_FAN));// set pins off
	
	// Port C
	DDRC = 0x00;			// analog pins are all input
	
	// Port D
	DDRD |= (1 << FLYBACK_BANK_1) | (1 << FLYBACK_BANK_2);		// set pins as output
	PORTD &= ~((1 << FLYBACK_BANK_1) | (1 << FLYBACK_BANK_2));	// set pins off
	
	///////////////////////
	
	// set all values in variables structure to zero
	uint8_t offset;
	for (offset = 0; offset < sizeof(struct variables_t); offset++)
		*((uint8_t *)&variables + offset) = 0;
}

int main(void) {
	dispatchInit();			// must be called 1st because other init function set and que tasks
	mainInit();				// must be called 2nd because it resets the variables structure
	uartInit();
	adcInit();
	fuelControlInit();
	
	configReadFromEeprom();	// load config variable from eeprom to ram
	tableReadFromEeprom(&veTable, &veTable_eeprom);			// load tables from eeprom to ram, see table.c
	tableReadFromEeprom(&advanceTable, &advanceTable_eeprom);
	
	timeInit();
	sei();					// enable interrupts
	
	/*** TESTING ***/
	queTask(TSK_ON_SPARK_EVENT);	// simulate spark interrupt
	/***************/
	
	for(;;) {
		dispatchTasks();
	}
}
