#ifndef _X86_BASIC_FUNCTIONS_H
#define _X86_BASIC_FUNCTIONS_H
#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) x86Output(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86Input(uint16_t port);

__attribute__((cdecl)) x86DiskReset(uint8_t disk);
__attribute__((cdecl)) x86DiskRead(uint8_t disk, uint16_t cylinder, uint16_t sector, uint16_t head, uint8_t count, void *lowerOutputData);
__attribute__((cdecl)) x86DiskGetParameters(uint8_t disk, uint8_t* outputDriveType, uint16_t* outputCylinders, uint16_t* outputSectors, uint16_t* outputHeads);

#endif
