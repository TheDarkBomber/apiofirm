#include "paging.h"
#include "x86.h"
#include <stdint.h>

#define PAGE_DIRECTORY 0x29C000
#define PAGE_SIZE 4096

uint32_t *PageDirectory = (uint32_t *)PAGE_DIRECTORY;
uint32_t *PageTable = (uint32_t *)(PAGE_DIRECTORY + 0x1000);
uint32_t PageAddress = 0;

void InitialisePaging() {
	for (uint16_t i = 0; i < 1024; i++) {
		PageTable[i] = PageAddress | 3;
		PageAddress += PAGE_SIZE;
	}

	PageDirectory[0] = PageTable;
	PageDirectory[0] |= 3;
	for (uint16_t i = 1; i < 1024; i++) PageDirectory[i] = 0 | 2;

	x86WriteCR3(PageDirectory);
	x86WriteCR0(x86ReadCR0() | 0x80000000);
}
