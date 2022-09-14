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
	PageCTX.PageBuffer = largestFreeSegment;
	memset(PageCTX.PageBuffer, 0, PageCTX.PageBufferLength);
	LockPages(PageCTX.PageBuffer, U64CeilingDivision(PageCTX.PageBufferLength, 4096));

	for (unsigned i = 0; i < entries; i++) {
		UefiMemoryDescriptor* descriptor = (UefiMemoryDescriptor*)((uint64_t)MMap + (i * mmapDSize));
		if (descriptor->Type != UefiConventionalMemory) ReservePages(descriptor->PhysicalAddress, descriptor->PageCount);
	}
}

static uint64_t LastPageBitIndex = 0;

char* RequestPage() {
	for (uint64_t bitIndex = LastPageBitIndex; bitIndex < PageCTX.PageBufferLength * 8; bitIndex++) {
		if (PageCTX.PageBuffer[bitIndex / 8] & (1 << (bitIndex % 8))) continue;
		LockPage((char*)(bitIndex * 4096));
		LastPageBitIndex = bitIndex;
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
	if (LastPageBitIndex > bitIndex) LastPageBitIndex = bitIndex;
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
	if (LastPageBitIndex > bitIndex) LastPageBitIndex = bitIndex;
}

void DereservePages(char* address, uint64_t amount) {
	for (unsigned i = 0; i < amount; i++) DereservePage(address + (i * 4096));
}

void SetPageMapIndex(PageMapIndex* PMI, uintptr_t virtualAddress) {
	virtualAddress = virtualAddress >> 12;
	PMI->PageIndex = virtualAddress & 0x1FF;
	virtualAddress = virtualAddress >> 9;
	PMI->PageTableIndex = virtualAddress & 0x1FF;
	virtualAddress = virtualAddress >> 9;
	PMI->PDIndex = virtualAddress & 0x1FF;
	virtualAddress = virtualAddress >> 9;
	PMI->PDPIndex = virtualAddress & 0x1FF;
}

PageTable* PML4;

void MapMemoryV2P(char* virtualAddress, char* physicalAddress) {
	PageMapIndex PMI;
	SetPageMapIndex(&PMI, (uintptr_t)virtualAddress);

	PageDirectoryEntry Entry;
	Entry = PML4->Entries[PMI.PDPIndex];
	PageTable* PDP;

	if (!Entry.Present) {
		PDP = (PageTable*)RequestPage();
		memset((char*)PDP, 0, 0x1000);
		Entry.Address = (uintptr_t)PDP >> 12;
		Entry.Present = 1;
		Entry.ReadWrite = 1;
		PML4->Entries[PMI.PDPIndex] = Entry;
	} else PDP = (PageTable*)((uintptr_t)Entry.Address << 12);

	Entry = PDP->Entries[PMI.PDIndex];
	PageTable* PageDirectory;
	if (!Entry.Present) {
		PageDirectory = (PageTable*)RequestPage();
		memset((char*)PageDirectory, 0, 0x1000);
		Entry.Address = (uintptr_t)PageDirectory >> 12;
		Entry.Present = 1;
		Entry.ReadWrite = 1;
		PDP->Entries[PMI.PDIndex] = Entry;
	} else PageDirectory = (PageTable*)((uintptr_t)Entry.Address << 12);

	Entry = PageDirectory->Entries[PMI.PageTableIndex];
	PageTable* PTable;
	if (!Entry.Present) {
		PTable = (PageTable*)RequestPage();
		memset((char*)PTable, 0, 0x1000);
		Entry.Address = (uintptr_t)PTable >> 12;
		Entry.Present = 1;
		Entry.ReadWrite = 1;
		PageDirectory->Entries[PMI.PageTableIndex] = Entry;
	} else PTable = (PageTable*)((uintptr_t)Entry.Address << 12);

	Entry = PTable->Entries[PMI.PageIndex];
	Entry.Address = (uintptr_t)physicalAddress >> 12;
	Entry.Present = 1;
	Entry.ReadWrite = 1;
	PTable->Entries[PMI.PageIndex] = Entry;
}

