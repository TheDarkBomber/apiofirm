#ifndef _X86_BASIC_FUNCTIONS_H
#define _X86_BASIC_FUNCTIONS_H
#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) x86Output(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86Input(uint16_t port);

void __attribute__((cdecl)) x86WriteCR0(uint32_t value);
void __attribute__((cdecl)) x86WriteCR3(uint32_t value);

uint32_t __attribute__((cdecl)) x86ReadCR0();
uint32_t __attribute__((cdecl)) x86ReadCR3();

#endif
