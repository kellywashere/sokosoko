#include "util.h"
#include <ctype.h>
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

/* Simple stack -- FUNCTIONS */
Stack* create_stack(unsigned int elemSize, unsigned int maxNrElems) {
	Stack* s = malloc(sizeof(Stack));
	if (s) {
		s->contents = malloc(elemSize * maxNrElems);
		s->elemSize = elemSize;
		s->maxNrElems = maxNrElems;
		empty_stack(s);
	}
	return s;
}

void destroy_stack(Stack* s) {
	if (s) {
		free(s->contents);
	}
	free(s);
}

void empty_stack(Stack* s) {
	s->nrElems = 0;
	s->top = s->contents;
}

void push_on_stack(Stack* s, void *el) {
	if (s->nrElems < s->maxNrElems) {
		memcpy(s->top, el, s->elemSize);
		s->top += s->elemSize;
		++s->nrElems;
	} else {
		fprintf(stderr, "push_on_stack: Stack overflow!\n");
		exit(1);
	}
}

void pop_from_stack(Stack* s, void *el) {
	if (s->nrElems == 0) {
		fprintf(stderr, "pop_from_stack: Stack is empty!\n");
		exit(1);
	}
	s->top -= s->elemSize;
	memcpy(el, s->top, s->elemSize);
	--s->nrElems;
}

bool is_stack_empty(Stack* s) {
	return (s->nrElems == 0);
}

// convenience stack fns for integer queue
Stack* create_int_stack(unsigned int maxNrElems) {
	return create_stack(sizeof(int), maxNrElems);
}

void push_on_int_stack(Stack* s, int x) {
	push_on_stack(s, &x);
}

int pop_from_int_stack(Stack* s) {
	int x;
	pop_from_stack(s, &x);
	return x;
}
/* END STACK */


int strcmpi(const char* str1, const char* str2) {
	for (int ii = 0; ; ++ii) {
		int d = tolower(str1[ii]) - tolower(str2[ii]);
		if (d != 0) {
			return d;
		}
		// str1[ii] == str2[ii]
		if (str1[ii] == 0) {
			return 0;
		}
	}
}



