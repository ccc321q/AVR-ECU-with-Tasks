
#include "uart.h"
#include "buffer.h"
#include "table.h"
#include "fuel_control.h"
#include "global.h"
#include "time.h"
#include "dispatch.h"
#include "adc.h"
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// command queue for output
struct cmd_queue_t cmd_queue[CMD_QUEUE_SIZE];
uint8_t cmd_queue_head, cmd_queue_tail;

// buffer for storing input
struct buffer_t uartRxBuffer;


/*** uartProcess  *********************************************************/
uint8_t uartProcess(uint8_t rx_data) {
	#define DEFAULT		0
	#define OFFSET_U	1
	#define OFFSET_L	2
	#define NEW_VALUE	3

	static uint8_t state = DEFAULT;
	static uint8_t offsetU = 0;
	static uint8_t offsetL = 0;
	static uint8_t command = 0;

	switch (state) {
	case OFFSET_U:
		// save upper byte of the 16bit offset for storing the new byte
		offsetU = rx_data;
		state = OFFSET_L;
		break;

	case OFFSET_L:
		// save lower byte of the 16bit offset for storing the new byte
		offsetL = rx_data;
		state = NEW_VALUE;
		break;
		
	case NEW_VALUE:
		switch (command) {
			case 'V':
				tableSaveValuesWithOffset(&veTable, &veTable_eeprom, offsetU, offsetL, rx_data);	// set new ve value
				state = DEFAULT; // back to normal mode
				break;
			case 'C':
				configVariablesSaveWithOffset(offsetL, rx_data);		// set new config value
				state = DEFAULT; // back to normal mode
				break;
			case 'X':
				tableSaveXaxisWithOffset(&veTable, &veTable_eeprom, offsetL, rx_data);
				state = DEFAULT; // back to normal mode
				break;
			case 'Y':
				tableSaveYaxisWithOffset(&veTable, &veTable_eeprom, offsetL, rx_data);
				state = DEFAULT; // back to normal mode
				break;
			default:
				state = DEFAULT; // back to normal mode
				break;
		}
		break;

	default:
		switch (rx_data) {
			case 'a':	// send all realtime values
				pushfunc(&variablesGet, sizeof(struct variables_t) + 1);
				break;
			case 'c':	// send all config values
				pushfunc(&configVariablesGetWithOffset, sizeof(struct config_t));
				break;
			case 'v':	// send ve table
				pushfunc(&veTableGetValuesWithOffset, TABLE_SIZE_X * TABLE_SIZE_Y * 2);		// * 2 because the table is 16 bit
				break;
			case 'x':	// send ve table x axis
				pushfunc(&veTableGetXaxisWithOffset, TABLE_SIZE_X * 2);		// * 2 because the x axis is 16 bit
				break;
			case 'y':	// send ve table y axis
				pushfunc(&veTableGetYaxisWithOffset, TABLE_SIZE_Y * 2);		// * 2 because the y axis is 16 bit
				break;
			case 'r':	// send adc values
				pushfunc(&getAnalog, ADC_CHANNELS);
				break;
			case 's':
				pushfunc(&increase, 1);
				break;
			case 'd':
				pushfunc(&decrease, 1);
				break;
			case 'V': // receive new VE at 'V'+<offsetU>+<offsetL>+<newbyte>
				state = OFFSET_U;
				command = 'V';
				break;
			case 'C': // receive new config value at 'C'+<offsetU>+<offsetL>+<newbyte>
				state = OFFSET_U;
				command = 'C';
				break;
			case 'X': // receive new X axis value for VE table at 'X'+<0>+<offsetL>+<newbyte>
				state = OFFSET_U;
				command = 'X';
				break;
			case 'Y': // receive new Y axis value for VE table at 'Y'+<0>+<offsetL>+<newbyte>
				state = OFFSET_U;
				command = 'Y';
				break;
			default:
				break;
		}
	}
	return 0;
}

uint8_t getAnalog(uint16_t byteLocation) {
	return adc[byteLocation];
}

uint8_t variablesGet(uint16_t byteLocation) {
	// return item in variables structure
	if (byteLocation == 0)
		return time.seconds;
	else if(byteLocation < (sizeof(struct variables_t) + 1))
		return *( (uint8_t *)&variables + byteLocation - 1);
	else
		return 0;
}

uint8_t increase(uint16_t lenght) {
	rpmSimulationtTicks += 30;
	return (uint8_t) rpmSimulationtTicks;
}

uint8_t decrease(uint16_t lenght) {
	rpmSimulationtTicks -= 30;
	return (uint8_t) rpmSimulationtTicks;
}

void uartProcessRx(void) {
	bufferProcess(&uartRxBuffer, &uartProcess);		// process uart recieve buffer
}

/*** pushfunc *************************************************************/
uint8_t pushfunc(uint8_t (*f)(uint16_t i), uint16_t len) {
	uint8_t next_head;

	next_head = (cmd_queue_head + 1) % CMD_QUEUE_SIZE;
	if (next_head == cmd_queue_tail)
		return -1; // whoops, queue is full

	/* len starts at 0 */
	if (len > 0) // this looks like a hack - it is!
		len = len - 1;

	cmd_queue[cmd_queue_head].func = f;
	cmd_queue[cmd_queue_head].len = len;
	cmd_queue[cmd_queue_head].count = 0;

	cmd_queue_head = next_head;
	
	UCSR0B |= _BV(UDRIE0); // enable transmit interrupt
	return 0;
}


/*** uartInit *************************************************************/
void uartInit(void) {
	setTask(TSK_UART_PROCESS_RX, &uartProcessRx);
	queTask(TSK_UART_PROCESS_RX);
	
	// 8 databit, 1 stopbit
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	
	// set baud rate, check uart.h for MYUBRR macro
	UBRR0H = ((MYUBRR)>>8);
    UBRR0L = MYUBRR;
    
	cmd_queue_head = cmd_queue_tail = 0;
	
	// initialize recieve buffer
	bufferInit(&uartRxBuffer);
	
	// rx enable, tx enable
	//  UCSR0B = _BV(RXCIE0) | _BV(UDRIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
}


/*** INTERRUPT: UART rx complete ******************************************/
ISR(USART_RX_vect) {

	// check status register for frame or overrun errors
	if ( UCSR0A & (_BV(FE0) | _BV(DOR0)) ) { 
		uint8_t dummy;
		dummy = UDR0; // just read uart and throw away the result
		return;
	} else {
		// put UDR0 in buffer here, process it from main loop
		bufferAppend(&uartRxBuffer, UDR0);
		//uartProcess(UDR0);
	}
}


/*** INTERRUPT: UART tx empty *********************************************/
ISR(USART_UDRE_vect) {

	//call function and get data to send
	UDR0 = (*cmd_queue[cmd_queue_tail].func)(cmd_queue[cmd_queue_tail].count);

	if (cmd_queue[cmd_queue_tail].count < cmd_queue[cmd_queue_tail].len) {
		cmd_queue[cmd_queue_tail].count++;
	} else {
		// we're done with this function

		cmd_queue_tail = (cmd_queue_tail + 1) % CMD_QUEUE_SIZE;
		if (cmd_queue_head == cmd_queue_tail) //buffer empty
			UCSR0B &= ~_BV(UDRIE0);             // disable TX interrupts
	}
}


