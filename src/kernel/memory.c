#include <memory.h>
#include <memory/heap.h>
#include <maths.h>
#include <stddef.h>

char* memcpy(char* destination, char* src, uint64_t size) {
	for (uint64_t i = 0; i < size; i++) destination[i] = src[i];
	return destination;
}

char* memset(char* pointer, char value, uint64_t size) {
	for (uint64_t i = 0; i < size; i++) pointer[i] = value;
	return pointer;
}

int memcmp(char* p1, char* p2, uint64_t size) {
	for (uint64_t i = 0; i < size; i++) {
		if (p1[i] != p2[i]) return 1;
	}

	return 0;
}

void mfree(char* addr) {
	HeapSegment* segment = (HeapSegment*)addr - 1;
	segment->Free = true;
	CombineHeapForward(segment);
	CombineHeapBackward(segment);
}

char* mallocate(uintptr_t size) {
	size = U64CeilingRound(size, 16);
	if (size == 0) return NULL;

	HeapSegment* currentSegment = (HeapSegment*)HeapCTX.Start;

	for(;;) {
		if (currentSegment->Free) {
			if (currentSegment->Length > size) {
				SplitHeapSegment(currentSegment, size);
				currentSegment->Free = false;
				return (char*)((uint64_t)currentSegment + sizeof(HeapSegment));
			}

			if (currentSegment->Length == size) {
				currentSegment->Free = false;
				return (char*)((uint64_t)currentSegment + sizeof(HeapSegment));
			}
		}
		if (currentSegment->Next == NULL) break;
		currentSegment = currentSegment->Next;
	}
	ExpandHeap(size);
	return mallocate(size);
}

char* mreallocate(char* addr, uintptr_t size) {
	if (addr) {
		char* newAddr = mallocate(size);
		HeapSegment* segment = (HeapSegment*)addr - 1;
		memcpy(newAddr, addr, segment->Length);
		segment->Free = true;
		CombineHeapForward(segment);
		CombineHeapBackward(segment);
		return newAddr;
	}
	return mallocate(size);
}

