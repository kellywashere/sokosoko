#include "queue.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Simple queue -- FUNCTIONS */
Queue* create_queue(unsigned int elemSize, unsigned int maxNrElems) {
	Queue* q = malloc(sizeof(Queue));
	if (q) {
		q->contents = malloc(elemSize * maxNrElems);
		q->elemSize = elemSize;
		q->maxNrElems = maxNrElems;
		empty_queue(q);
	}
	return q;
}

void destroy_queue(Queue* q) {
	if (q) {
		free(q->contents);
	}
	free(q);
}

void empty_queue(Queue* q) {
	q->nrElems = 0;
	q->front = q->contents;
	q->back = q->contents;
}

void enQueue(Queue* q, void *el) {
	if (q->nrElems < q->maxNrElems) {
		if (q->back <= q->contents) {
			q->back += q->elemSize * q->maxNrElems;
		}
		q->back -= q->elemSize;
		memcpy(q->back, el, q->elemSize);
		++q->nrElems;
	} else {
		fprintf(stderr, "enQueue: Queue is overflowing!\n");
		exit(1);
	}
}

void deQueue(Queue* q, void *el) {
	if (q->nrElems == 0) {
		fprintf(stderr, "deQueue: Queue is empty!\n");
		exit(1);
	}
	if (q->front <= q->contents) {
		q->front += q->elemSize * q->maxNrElems;
	}
	q->front -= q->elemSize;
	memcpy(el, q->front, q->elemSize);
	--q->nrElems;
};

bool is_queue_empty(Queue* q) {
	return (q->nrElems == 0);
}

// convenience queue fns for integer queue
Queue* create_int_queue(unsigned int maxNrElems) {
	return create_queue(sizeof(int), maxNrElems);
}

void enQueue_int(Queue* q, int x) {
	enQueue(q, &x);
}

int deQueue_int(Queue* q) {
	int x;
	deQueue(q, &x);
	return x;
}
/* END QUEUE */

