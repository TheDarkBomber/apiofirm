#ifndef __APIOFIRM_IDT_H_
#define __APIOFIRM_IDT_H_
#include <stdint.h>

typedef enum {
	IDT_InterruptGate = 0b10001110,
	IDT_CallGate = 0b10001100,
	IDT_TrapGate = 0b10001111
} IDTGate;

typedef struct {
	uint16_t LowBase;
	uint16_t KernelCS;
	uint8_t IST;
	uint8_t Attributes;
	uint16_t IntermediaryBase;
	uint32_t HighBase;
	uint32_t Reserved;
} __attribute__((packed)) IDTEntry;

typedef struct {
	uint16_t Limit;
	uint64_t Base;
} __attribute__((packed)) MetaIDT;

void InitialiseInterrupts();
void SetIDTDescriptor(uint8_t vector, void* ISR, uint8_t flags);

#endif
