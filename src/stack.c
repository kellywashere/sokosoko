#include "stack.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

