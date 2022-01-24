#ifndef _X86_BASIC_FUNCTIONS_H
#define _X86_BASIC_FUNCTIONS_H
#include "stdint.h"

void _cdecl x86Divide64By32(uint64_t numerator, uint32_t denominator, uint64_t* outputQuotient, uint32_t* outputRemainder);
void _cdecl x86TeletypeModeWriteCharacter(char c, uint8_t page);

bool _cdecl x86DiskReset(uint8_t disk);
bool _cdecl x86DiskRead(uint8_t disk, uint16_t cylinder, uint16_t sector,
                        uint16_t head, uint8_t count, void far *outputData);
bool _cdecl x86DiskGetParameters(uint8_t disk, uint8_t* outputDriveType, uint16_t* outputCylinders, uint16_t* outputSectors, uint16_t* outputHeads);

#endif
