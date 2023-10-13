#ifndef HEAP_H
#define HEAP_H

#include "../source/typeDef.h"

//each allocated chunk saves an header before the actual data
struct MemorySegmentHeader {
	uint_64 MemoryLength;
	struct MemorySegmentHeader* NextSegment;
	struct MemorySegmentHeader* PreviousSegment;
	struct MemorySegmentHeader* NextFreeSegment;
	struct MemorySegmentHeader* PreviousFreeSegment;
	bool Free;
};

typedef struct MemorySegmentHeader MemorySegmentHeader;

void InitializeHeap(uint_64 heapAddress, uint_64 heapLength);
void* malloc(uint_64 size);			//allocating memory on heap
void* realloc(void* address, uint_64 newSize); //relocation memory on heap
void free(void* address);			//free the allocated memory

#endif