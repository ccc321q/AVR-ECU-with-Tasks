
/**********************************************
 * Task scheduling and dispaching
 * By Chrisiaan Aspeling
 * 
 * Once tasks are queued they execute until they
 * are unqueued. Tasks can be queued with a delay
 * that is decremented everytime dispatchTick()
 * is called.
 * 
 **********************************************/

#include "dispatch.h"

static struct {
	uint8_t active;					// should task be executed? 0 = false, 1 = true
	uint16_t delay;					// delay in ticks
	void (*funtionPointer)(void);	// pointer to task
} task[TOTALTASKS];


void dispatchInit(void) {
	uint8_t i;
	for (i = 0; i < TOTALTASKS; i++) {
		task[i].active = 0;
		task[i].delay = 0;
		task[i].funtionPointer = 0;
	}
}

void dispatchTasks(void) {
	uint8_t i;
	for (i = 0; i < TOTALTASKS; i++) {
		if (task[i].active) {
			if (task[i].delay <= 0) {
				(*task[i].funtionPointer)();	// Call task function
			}
		}
	}
}

void dispatchTick(void) {
	uint8_t i;
	for(i = 0; i < TOTALTASKS; i++) {	// For every task in schedule
		if (task[i].active) {
			if( !(task[i].delay <= 0) ) {	// Task is ready when task_delay = 0
				task[i].delay--;			// task delay decremented until it reaches zero (time to run)
			}
		}
	}
}

void queTask(uint8_t taskID) {
	task[taskID].active = 1;
	task[taskID].delay = 0;
}

void queTaskDelayed(uint8_t taskID, uint16_t delay_) {
	task[taskID].active = 1;
	task[taskID].delay = delay_;
}

void unQueTask(uint8_t taskID) {
	task[taskID].active = 0;
}

void setTask(uint8_t taskID, void (*pointer_to_function)(void)) {
	task[taskID].funtionPointer = pointer_to_function;
}
