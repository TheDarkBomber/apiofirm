#ifndef __MEMORY_STD_H
#define __MEMORY_STD_H
#include <stdint.h>

void *memcpy(void *destination, const void *source,
                 uint16_t number);
void *memset(void *pointer, int value, uint16_t number);
int memcmp(const void * pointer1, const void * pointer2, uint16_t number);
#endif
