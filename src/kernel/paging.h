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

typedef struct {
	unsigned Present : 1;
	unsigned ReadWrite : 1;
	unsigned Userspace : 1;
	unsigned WriteThrough : 1;
	unsigned CacheDisabled : 1;
	unsigned Accessed : 1;
	unsigned RSVP0 : 1;
	unsigned LargerPages : 1;
	unsigned RSVP1 : 1;
	unsigned Available : 3;
	uint64_t Address: 52;
} PageDirectoryEntry;

typedef struct {
	PageDirectoryEntry Entries[512];
} __attribute__((aligned(0x1000))) PageTable;

typedef struct {
	uint64_t PDPIndex;
	uint64_t PDIndex;
	uint64_t PageTableIndex;
	uint64_t PageIndex;
} PageMapIndex;

void SetPageMapIndex(PageMapIndex* PMI, uintptr_t virtualAddress);

extern PageTable* PML4;

void MapMemoryV2P(char* virtualAddress, char* physicalAddress);

#endif
