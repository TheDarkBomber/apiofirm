#ifndef __MEMORY_STD_H
#define __MEMORY_STD_H
#include "stdint.h"

void far *memcpy(void far *destination, const void far *source,
                 uint16_t number);
void far *memset(void far *pointer, int value, uint16_t number);
int memcmp(const void far* pointer1, const void far* pointer2, uint16_t number);
#endif
