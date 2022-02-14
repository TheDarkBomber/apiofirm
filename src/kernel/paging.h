#ifndef __PAGING_MEMORY_KERNEL_H
#define __PAGING_MEMORY_KERNEL_H
#include <stdint.h>

#define PAGE_DIRECTORY 0x29C000
#define PAGE_ORIGIN (PAGE_DIRECTORY + 0x1000)
#define PAGE_SIZE 4096
#define PAGE_BLOCK 0x400000

void InitialisePaging();
void AllocatePage(uint16_t index);
void FreePage(uint16_t index);

#endif
