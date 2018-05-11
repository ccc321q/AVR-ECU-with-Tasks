
#ifndef UART_H
#define UART_H

#include <inttypes.h>

#define F_CPU 16000000UL			// clock speed, 16MHz
//#define BAUD 9600
#define BAUD 57600					// desired baud rate
//#define BAUD 115200
#define MYUBRR F_CPU/16/BAUD-1		// used to set UBRR0H and UBRR0L which sets the baud rate

#define CMD_QUEUE_SIZE 32

struct cmd_queue_t {
	uint8_t (*func)(uint16_t i);
	uint16_t len;
	uint16_t count;
};

void uartInit(void);
uint8_t uartProcess(uint8_t data);
void uartProcessRx(void);
uint8_t pushfunc(uint8_t (*f)(uint16_t i), uint16_t len);

void configVariablesSet(uint8_t byteLocation, uint8_t newByte);
uint8_t configVariablesGet(uint16_t byteLocation);

uint8_t variablesGet(uint16_t byteLocation);
uint8_t getAnalog(uint16_t byteLocation);

uint8_t increase(uint16_t lenght);
uint8_t decrease(uint16_t lenght);

#endif
