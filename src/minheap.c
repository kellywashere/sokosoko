#include "minheap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

MinHeap* create_minheap(unsigned int dataSize) {
	MinHeap* heap = malloc(sizeof(MinHeap));
	if (heap) {
		heap->maxNrElems = 127; // 7 layers start size
		heap->mem = malloc( (sizeof(int) + dataSize) * heap->maxNrElems );
		heap->dataSize = dataSize;
		heap->nrElems = 0;
		heap->swapElem = malloc(sizeof(int) + dataSize);
	}
	return heap;
}

void destroy_minheap(MinHeap* heap) {
	if (heap) {
		free(heap->mem);
		free(heap->swapElem);
	}
	free(heap);
}

static void* minheap_idx2addr(MinHeap* heap, int idx) {
	return heap->mem + idx * (sizeof(int) + heap->dataSize);
}

static void minheap_swap(MinHeap* heap, int idx1, int idx2) {
	void* p1 = minheap_idx2addr(heap, idx1);
	void* p2 = minheap_idx2addr(heap, idx2);
	int sz = sizeof(int) + heap->dataSize;
	memcpy(heap->swapElem, p1, sz);
	memcpy(p1, p2, sz);
	memcpy(p2, heap->swapElem, sz);
}

static int minheap_getkey(MinHeap* heap, int idx) {
	int* pKey = minheap_idx2addr(heap, idx);
	return *pKey;
}

static void minheap_bubble_up(MinHeap* heap, int idx) {
	int idx_parent = (idx - 1) / 2;
	while (idx != 0 && minheap_getkey(heap, idx) < minheap_getkey(heap, idx_parent)) {
		minheap_swap(heap, idx, idx_parent);
		idx = idx_parent;
		idx_parent = (idx - 1) / 2;
	}
}

static void minheap_bubble_down(MinHeap* heap) {
	int idx = 0;
	int key_this = minheap_getkey(heap, idx);
	bool done = false;
	while (!done) {
		done = true;
		int smallest_key = key_this;
		int idx_smallest = idx;
		// find smallest between idx, left and right child
		for (int idx_child = (idx * 2) + 1; idx_child <= (idx * 2) + 2; ++idx_child) {
			if (idx_child < heap->nrElems) {
				int k = minheap_getkey(heap, idx_child);
				if (k < smallest_key) {
					smallest_key = k;
					idx_smallest = idx_child;
				}
			}
		}
		if (idx_smallest != idx) {
			done = false;
			minheap_swap(heap, idx, idx_smallest);
			idx = idx_smallest;
		}
	}
}

void minheap_insert(MinHeap* heap, int key, void* pData) {
	// check capacity
	if (heap->nrElems == heap->maxNrElems) {
		heap->maxNrElems = 2 * heap->maxNrElems + 1; // 1 more layer
		heap->mem = realloc(heap->mem, (sizeof(int) + heap->dataSize) * heap->maxNrElems );
	}
	int idx = heap->nrElems; // insertion idx
	void* p = minheap_idx2addr(heap, idx);
	// copy info into new last element
	memcpy(p, &key, sizeof(int));
	memcpy(p + sizeof(int), pData, heap->dataSize);
	++heap->nrElems;
	minheap_bubble_up(heap, idx);
}

int minheap_extract(MinHeap* heap, void* pData) { // returns key, copies data to pData
	if (heap->nrElems == 0) {
		fprintf(stderr, "Error: extracting from empty heap\n");
		exit(1);
	}
	int retKey = minheap_getkey(heap, 0);
	if (pData) {
		memcpy(pData, heap->mem + sizeof(int), heap->dataSize);
	}

	--heap->nrElems;
	if (heap->nrElems > 0) {
		// copy ex-last element to root position
		memcpy(minheap_idx2addr(heap, 0), minheap_idx2addr(heap, heap->nrElems),
				sizeof(int) + heap->dataSize);
		minheap_bubble_down(heap);
	}
	return retKey;
}

bool is_minheap_empty(MinHeap* heap) {
	return heap->nrElems == 0;
}
