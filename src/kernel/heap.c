#include "heap.h"
#include "paging.h"
#include "maths.h"
#include <stddef.h>

Heap HeapCTX;

void InitialiseHeap(char* addr, uintptr_t heapSizeInPages) {
	char* pageAddr = addr;
	for (uintptr_t i = 0; i < heapSizeInPages; i++) {
		MapMemoryV2P(pageAddr, RequestPage());
		pageAddr = (char*)((uintptr_t)pageAddr + 4096);
	}

	HeapCTX.Start = addr;
	HeapCTX.End = (char*)((uintptr_t)HeapCTX.Start + heapSizeInPages * 4096);
	HeapSegment* initialSegment = (HeapSegment*)addr;
	initialSegment->Length = (heapSizeInPages * 4096) - sizeof(HeapSegment);
	initialSegment->Next = NULL;
	initialSegment->Last = NULL;
	initialSegment->Free = true;
	HeapCTX.LastSegment = initialSegment;
}

void ExpandHeap(uintptr_t length) {
	length = U64CeilingRound(length, 4096);
	uintptr_t pages = length / 4096;
	HeapSegment* newSegment = (HeapSegment*)HeapCTX.End;

	for (uintptr_t i = 0; i < pages; i++) {
		MapMemoryV2P(HeapCTX.End, RequestPage());
		HeapCTX.End = (char*)((uintptr_t)HeapCTX.End + 4096);
	}

	newSegment->Free = true;
	newSegment->Last = HeapCTX.LastSegment;
	HeapCTX.LastSegment->Next = newSegment;
	HeapCTX.LastSegment = newSegment;
	newSegment->Next = NULL;
	newSegment->Length = length - sizeof(HeapSegment);
	CombineHeapBackward(newSegment);
}

void CombineHeapForward(HeapSegment* segment) {
	if (segment->Next == NULL) return;
	if (!segment->Next->Free) return;
	if (segment->Next == HeapCTX.LastSegment) HeapCTX.LastSegment = segment;
	if (segment->Next->Next != NULL) segment->Next->Next->Last = segment;

	segment->Length = segment->Length + segment->Next->Length + sizeof(HeapSegment);
	segment->Next = segment->Next->Next;
}

void CombineHeapBackward(HeapSegment* segment) {
	if (segment->Last != NULL && segment->Last->Free) CombineHeapForward(segment->Last);
}

HeapSegment* SplitHeapSegment(HeapSegment* segment, uintptr_t splitLength) {
	if (splitLength < 16) return NULL;
	int64_t splitSegmentLength = segment->Length - splitLength - sizeof(HeapSegment);
	if (splitSegmentLength < 16) return NULL;

	HeapSegment* newSplitSegment = (HeapSegment*)((uintptr_t)segment + splitLength + sizeof(HeapSegment));
	segment->Next->Last = newSplitSegment;
	newSplitSegment->Next = segment->Next;
	segment->Next = newSplitSegment;
	newSplitSegment->Last = segment;
	newSplitSegment->Length = splitSegmentLength;
	newSplitSegment->Free = segment->Free;
	segment->Length = splitLength;

	if (HeapCTX.LastSegment == segment) HeapCTX.LastSegment = newSplitSegment;
	return newSplitSegment;
}
