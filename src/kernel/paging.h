#ifndef __APIOFIRM_PAGING_H_
#define __APIOFIRM_PAGING_H_
#include "uefi-mmap.h"
#include <stdint.h>

typedef struct {
	char* PageBuffer;
	uint64_t TotalMemorySize;
	uint64_t FreeMemorySize;
	uint64_t ReservedMemorySize;
	uint64_t UsedMemorySize;
	uint64_t PageBufferLength;
} PageFrameAllocatorContext;

extern PageFrameAllocatorContext PageCTX;

void InitialisePFA(UefiMemoryDescriptor* MMap, uintptr_t mmapSize, uintptr_t mmapDSize);
char* RequestPage();
void FreePage(char* address);
void FreePages(char* address, uint64_t amount);
void LockPage(char* address);
void LockPages(char* address, uint64_t amount);
void ReservePage(char* address);
void ReservePages(char* address, uint64_t amount);
void DereservePage(char* address);
void DereservePages(char* address, uint64_t amount);

#endif
