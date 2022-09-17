#ifndef __APIOFIRM_HEAP_H_
#define __APIOFIRM_HEAP_H_
#include <stdint.h>
#include <stdbool.h>

typedef struct HeapSegment_ {
	bool Free;
	uintptr_t Length;
	struct HeapSegment_* Next;
	struct HeapSegment_* Last;
} HeapSegment;

typedef struct {
  char* Start;
	char* End;
	HeapSegment* LastSegment;
} Heap;

void InitialiseHeap(char* addr, uintptr_t heapSizeInPages);
void ExpandHeap(uintptr_t length);
void CombineHeapForward(HeapSegment* segment);
void CombineHeapBackward(HeapSegment* segment);
HeapSegment* SplitHeapSegment(HeapSegment* segment, uintptr_t splitLength);

extern Heap HeapCTX;

#endif
