#ifndef DISPATCH_H
#define DISPATCH_H

#include <inttypes.h>

#define TOTALTASKS 7

// allocate each task a position in the task array
#define TSK_ON_TIME_TICK_EVENT 	0
#define TSK_INJBANK1_OFF		1
#define TSK_INJBANK2_OFF		2
#define TSK_ON_SPARK_EVENT		3
#define TSK_UART_PROCESS_RX		4	// runs non stop
#define TSK_ADC_READALL_START	5
#define TSK_

void dispatchInit(void);
void dispatchTasks(void);
void dispatchTick(void);
void queTask(uint8_t taskID);
void queTaskDelayed(uint8_t taskID, uint16_t delay_);
void unQueTask(uint8_t taskID);
void setTask(uint8_t taskID, void (*pointer_to_function)(void));

#endif
