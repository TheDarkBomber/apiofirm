#ifndef __APIOFIRM_MEMORY_H_
#define __APIOFIRM_MEMORY_H_
#include <stdint.h>

char* memcpy(char* destination, char* src, uint64_t size);
char* memset(char* pointer, char value, uint64_t size);
int memcmp(char* p1, char* p2, uint64_t size);

void mfree(char* addr);
char* mallocate(uintptr_t size);

#endif
