#include "acpi.h"
#include "memory.h"
#include <stddef.h>

SystemDescriptorTable* ACPIFindTable(SystemDescriptorTable* SDT, char* signature) {
	unsigned entries = (SDT->Length - sizeof(SystemDescriptorTable)) / 8;

	for (int i = 0; i < entries; i++) {
		SystemDescriptorTable* NewSDT = (SystemDescriptorTable*)*(uint64_t*)((uint64_t)SDT + sizeof(SystemDescriptorTable) + (i * 8));
		if (!memcmp((char*)NewSDT->Signature, signature, 4)) return NewSDT;
	}

	return NULL;
}
