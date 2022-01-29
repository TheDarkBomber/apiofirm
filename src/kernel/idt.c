#include "idt.h"
#include <stdint.h>

#define IDT_SIZE 256
#define IDT_INTERRUPTS 0x30

__attribute__((aligned(0x10)))
static IDTEntry InterruptDescriptorTable[IDT_SIZE];

typedef struct {
	uint16_t LIMIT;
	uint32_t BASE;
} __attribute__((packed)) IDTRegister;

static IDTRegister IDTR;

void IDTSetDescriptor(uint8_t vector, void *ISR, uint8_t flags) {
	IDTEntry* descriptor = &InterruptDescriptorTable[vector];

	descriptor->ISR_LOW = (uint32_t)ISR & 0xFFFF;
	descriptor->KERNEL_CODE_SELECTOR = 0x08;
	descriptor->ATTRIBUTES = flags;
	descriptor->ISR_HIGH = (uint32_t)ISR >> 16;
	descriptor->RSVP = 0;
}

extern void* ISRStubTable[];
void InitialiseIDT() {
	IDTR.BASE = (uintptr_t)&InterruptDescriptorTable[0];
	IDTR.LIMIT = (uint16_t)sizeof(IDTEntry) * IDT_SIZE - 1;

	for (uint8_t vector = 0; vector < IDT_INTERRUPTS; vector++) {
		IDTSetDescriptor(vector, ISRStubTable[vector], 0x8E);
	}

	__asm__ volatile ("lidt %0" : : "m"(IDTR));
}
