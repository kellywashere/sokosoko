#ifndef __MINHEAP_H__
#define __MINHEAP_H__

#include <stdbool.h>

typedef struct MinHeap {
	void*        mem;        // array-based min heap, manual memory management (size element unknown)
	unsigned int dataSize;   // size [B] of heap element not including the key int (payload only)
	unsigned int nrElems;    // nr elements currently in heap
	unsigned int maxNrElems; // capacity in nr of elements
	void*        swapElem;   // room for swapping
} MinHeap;

MinHeap* create_minheap(unsigned int dataSize);
void destroy_minheap(MinHeap* heap);

void minheap_insert(MinHeap* heap, int key, void* pData);
int  minheap_extract(MinHeap* heap, void* pData); // returns key, copies data to pData
bool is_minheap_empty(MinHeap* heap);

// TODO: heapsort
#endif

