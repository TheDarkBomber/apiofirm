#ifndef __REGISTERS_CPU_H
#define __REGISTERS_CPU_H
#include <stdint.h>

typedef struct {
	uint32_t EAX;
	uint32_t EBX;
	uint32_t ECX;
	uint32_t EDX;
	uint32_t ESI;
	uint32_t EDI;
	uint32_t ESP;
	uint32_t EBP;
	uint32_t EIP;
	uint32_t EFLAGS;
	uint32_t CR3;
} __attribute__((packed)) Registers;
#endif
