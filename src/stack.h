#ifndef __STACK_H__
#define __STACK_H__
#include <stdbool.h>

// Simple stack implementation
// Stack: general purpose, uses void* to point to elements on stack; copy is made of data
// Some convenience functions are provided for working with stack of int

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
void push_on_stack(Stack* s, void* el);   // adds element *el to stack
void pop_from_stack(Stack* s, void* el);  // removes element from stack, sticks it in *el
bool is_stack_empty(Stack* s);
// convenience stack fns for integer queue
Stack* create_int_stack(unsigned int maxNrElems);
void push_on_int_stack(Stack* s, int x);
int pop_from_int_stack(Stack* s);

#endif
