
#ifndef BUFFER_H
#define BUFFER_H

#include <inttypes.h>

#define BUFFERLEN	32

struct buffer_t {
  uint8_t head;		// head index
  uint8_t tail;		// tail index
  uint8_t count;	// number of items in buffer
  uint8_t buffer[BUFFERLEN]; // the actual buffer contents
};

void bufferInit(struct buffer_t *buf);
void bufferProcess(struct buffer_t *buf, uint8_t (*func)(uint8_t i));
__inline__ void bufferAppend(struct buffer_t *buf, uint8_t value);

#endif
