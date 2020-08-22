#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdbool.h>

typedef struct Queue {
	void*        contents;   // array of general element
	unsigned int elemSize;   // nrElems of each element in bytes
	unsigned int maxNrElems; // maximum nr of elements in queue
	unsigned int nrElems;    // current nr of elements in queue
	void*        front;      // front of queue
	void*        back;       // back of queue
} Queue;

Queue* create_queue(unsigned int elemSize, unsigned int maxNrElems);
void destroy_queue(Queue* q);
void empty_queue(Queue* q);         // empties the queue
void enQueue(Queue* q, void *el);  // adds element *el to queue
void deQueue(Queue* q, void *el);  // removes element from queue, sticks it in *el
bool is_queue_empty(Queue* q);
// convenience queue fns for integer queue
Queue* create_int_queue(unsigned int maxNrElems);
void enQueue_int(Queue* q, int x);
int deQueue_int(Queue* q);

#endif
