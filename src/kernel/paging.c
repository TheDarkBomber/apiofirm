#include "paging.h"
#include "memory.h"
#include "maths.h"
#include <stddef.h>

PageFrameAllocatorContext PageCTX;

void InitialisePFA(UefiMemoryDescriptor* MMap, uintptr_t mmapSize, uintptr_t mmapDSize) {
	uint64_t entries = mmapSize / mmapDSize;
	char* largestFreeSegment = NULL;
	uintptr_t largestFreeSize = 0;

	for (unsigned i = 0; i < entries; i++) {
		UefiMemoryDescriptor* descriptor = (UefiMemoryDescriptor*)((uint64_t)MMap + (i * mmapDSize));
		if (descriptor->Type == UefiConventionalMemory) {
			if (descriptor->PageCount * 4096 > largestFreeSize) {
				largestFreeSegment = descriptor->PhysicalAddress;
				largestFreeSize = descriptor->PageCount * 4096;
			}
		}
	}

	PageCTX.TotalMemorySize = PageCTX.FreeMemorySize = GetTotalMemorySize(MMap, entries, mmapDSize);
	PageCTX.PageBufferLength = U64CeilingDivision(PageCTX.TotalMemorySize, 4096 / 8);
	memset(PageCTX.PageBuffer, 0, PageCTX.PageBufferLength);
	LockPages(PageCTX.PageBuffer, U64CeilingDivision(PageCTX.PageBufferLength, 4096));

	for (unsigned i = 0; i < entries; i++) {
		UefiMemoryDescriptor* descriptor = (UefiMemoryDescriptor*)((uint64_t)MMap + (i * mmapDSize));
		if (descriptor->Type != UefiConventionalMemory) ReservePages(descriptor->PhysicalAddress, descriptor->PageCount);
	}
}

char* RequestPage() {
	for (uint64_t bitIndex = 0; bitIndex < PageCTX.PageBufferLength * 8; bitIndex++) {
		if (PageCTX.PageBuffer[bitIndex / 8] & (1 << (bitIndex % 8))) continue;
		LockPage((char*)(bitIndex * 4096));
		return (char*)(bitIndex * 4096);
	}
	return NULL;
}

void FreePage(char* address) {
	uint64_t bitIndex = (uint64_t)address / 4096;
	if (!(PageCTX.PageBuffer[bitIndex / 8] & (1 << (bitIndex % 8)))) return;
	PageCTX.PageBuffer[bitIndex / 8] &= ~(1 << (bitIndex % 8));
	PageCTX.FreeMemorySize += 4096;
	PageCTX.UsedMemorySize -= 4096;
}

void FreePages(char* address, uint64_t amount) {
	for (unsigned i = 0; i < amount; i++) FreePage(address + (i * 4096));
}

void LockPage(char* address) {
	uint64_t bitIndex = (uint64_t)address / 4096;
	if (PageCTX.PageBuffer[bitIndex / 8] & (1 << (bitIndex % 8))) return;
	PageCTX.PageBuffer[bitIndex / 8] |= (1 << (bitIndex % 8));
	PageCTX.FreeMemorySize -= 4096;
	PageCTX.UsedMemorySize += 4096;
}

void LockPages(char* address, uint64_t amount) {
	for (unsigned i = 0; i < amount; i++) LockPage(address + (i * 4096));
}

void ReservePage(char* address) {
	uint64_t bitIndex = (uint64_t)address / 4096;
	if (PageCTX.PageBuffer[bitIndex / 8] & (1 << (bitIndex % 8))) return;
	PageCTX.PageBuffer[bitIndex / 8] |= (1 << (bitIndex % 8));
	PageCTX.FreeMemorySize -= 4096;
	PageCTX.ReservedMemorySize += 4096;
}

void ReservePages(char* address, uint64_t amount) {
	for (unsigned i = 0; i < amount; i++) ReservePage(address + (i * 4096));
}

void DereservePage(char* address) {
	uint64_t bitIndex = (uint64_t)address / 4096;
	if (!(PageCTX.PageBuffer[bitIndex / 8] & (1 << (bitIndex % 8)))) return;
	PageCTX.PageBuffer[bitIndex / 8] &= ~(1 << (bitIndex % 8));
	PageCTX.FreeMemorySize += 4096;
	PageCTX.ReservedMemorySize -= 4096;
}

void DereservePages(char* address, uint64_t amount) {
	for (unsigned i = 0; i < amount; i++) DereservePage(address + (i * 4096));
}
