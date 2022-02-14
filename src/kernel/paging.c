#include "paging.h"
#include "x86.h"
#include <stdint.h>

uint32_t *PageDirectory = (uint32_t *)PAGE_DIRECTORY;
uint32_t *PageTable[1024] = {(uint32_t *)PAGE_ORIGIN};
uint32_t PageAddress = 0;

void InitialisePaging() {
	for (uint16_t i = 0; i < 1024; i++) {
		PageTable[0][i] = PageAddress | 3;
		PageAddress += PAGE_SIZE;
	}

	PageDirectory[0] = PageTable[0];
	PageDirectory[0] |= 3;
	for (uint16_t i = 1; i < 1024; i++) PageDirectory[i] = 0 | 2;

	x86WriteCR3(PageDirectory);
	x86WriteCR0(x86ReadCR0() | 0x80000000);
}

void AllocatePage(uint16_t index) {
	PageAddress = 0;
	PageTable[index] = (uint32_t *)(PAGE_ORIGIN + index * 0x1000);
	for (uint16_t i = 0; i < 1024; i++) {
		PageTable[index][i] = PageAddress | 3;
		PageAddress += PAGE_SIZE;
	}

	PageDirectory[index] = PageTable[index];
	PageDirectory[index] |= 3;
}

void FreePage(uint16_t index) {
	if (!PageTable[index]) return;
	for (uint16_t i = 0; i < 1024; i++) PageTable[index][i] = 0;
	PageDirectory[index] = 0 | 2;
}
