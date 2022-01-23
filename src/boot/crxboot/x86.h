#ifndef _X86_BASIC_FUNCTIONS_H
#define _X86_BASIC_FUNCTIONS_H
#include "stdint.h"

void _cdecl x86Divide64By32(uint64_t numerator, uint32_t denominator, uint64_t* outputQuotient, uint32_t* outputRemainder);
void _cdecl x86TeletypeModeWriteCharacter(char c, uint8_t page);

#endif
