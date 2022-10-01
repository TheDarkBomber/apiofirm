#include <int/idt.h>
#include <memory.h>

extern void* ISRStubTable[];

IDTEntry IDT[256];
MetaIDT metaIDT;

static IDTGate ISRGateTable[0x30];

void SetIDTDescriptor(uint8_t vector, void* ISR, uint8_t flags) {
	IDTEntry* descriptor = &IDT[vector];

	descriptor->LowBase = (uint64_t)ISR & 0xFFFF;
	descriptor->KernelCS = 0x08;
	descriptor->IST = 0;
	descriptor->Attributes = flags;
	descriptor->IntermediaryBase = ((uint64_t)ISR >> 16) & 0xFFFF;
	descriptor->HighBase = ((uint64_t)ISR >> 32) & 0xFFFFFFFF;
	descriptor->Reserved = 0;
}

void InitialiseInterrupts() {
	metaIDT.Base = (uintptr_t)&IDT[0];
	metaIDT.Limit = (uint16_t)sizeof(IDTEntry) * 256 - 1;

	memset((char*)ISRGateTable, (char)IDT_InterruptGate, 0x30);

	ISRGateTable[0x01] = IDT_TrapGate;
	ISRGateTable[0x03] = IDT_TrapGate;
	ISRGateTable[0x04] = IDT_TrapGate;

	for (uint8_t v = 0; v < 0x30; v++) {
		SetIDTDescriptor(v, ISRStubTable[v], 0x8E);
	}

	asm ("lidt %0" : : "m"(metaIDT));
	asm ("sti");
}
