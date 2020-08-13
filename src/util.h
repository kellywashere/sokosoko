#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdbool.h>

#define MAX(A,B) (((A)>(B))?(A):(B))
#define MIN(A,B) (((A)<(B))?(A):(B))

int strcmpi(const char* str1, const char* str2);

/* Simple queue */ 
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

/* Simple queue */ 
typedef struct Stack {
	void*        contents;   // array of general element
	unsigned int elemSize;   // nrElems of each element in bytes
	unsigned int maxNrElems; // maximum nr of elements in stack
	unsigned int nrElems;    // current nr of elements in stack
	void*        top;        // top of stack
} Stack;

Stack* create_stack(unsigned int elemSize, unsigned int maxNrElems);
void destroy_stack(Stack* s);
void empty_stack(Stack* s);         // empties the stack
void push_on_stack(Stack* s, void *el);   // adds element *el to stack
void pop_from_stack(Stack* s, void *el);  // removes element from stack, sticks it in *el
bool is_stack_empty(Stack* s);
// convenience stack fns for integer queue
Stack* create_int_stack(unsigned int maxNrElems);
void push_on_int_stack(Stack* s, int x);
int pop_from_int_stack(Stack* s);

#endif
