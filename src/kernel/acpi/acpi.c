#include <acpi/acpi.h>
#include <memory.h>
#include <stddef.h>

static uintptr_t ACPI_XSDT;

void ACPISetXSDT(SystemDescriptorTable *XSDT) { ACPI_XSDT = (uintptr_t)XSDT; }

SystemDescriptorTable* ACPIFindTable(SystemDescriptorTable* SDT, char* signature) {
	unsigned entries = (SDT->Length - sizeof(SystemDescriptorTable)) / 8;

	for (int i = 0; i < entries; i++) {
		SystemDescriptorTable* NewSDT = (SystemDescriptorTable*)*(uint64_t*)((uint64_t)SDT + sizeof(SystemDescriptorTable) + (i * 8));
		if (!memcmp((char*)NewSDT->Signature, signature, 4)) return NewSDT;
	}

	return NULL;
}

uintptr_t ACPIGetTableAddress(char* signature, uintptr_t index) {
	SystemDescriptorTable* SDT;
	SystemDescriptorTable* XSDT = (SystemDescriptorTable*)ACPI_XSDT;
	int count = 0;
	for (uintptr_t i = 0; i < (XSDT->Length - sizeof(SystemDescriptorTable)) / 8; i++) {
		SDT = (SystemDescriptorTable*)*(uint64_t*)((uint64_t)XSDT + sizeof(SystemDescriptorTable) + (i * 8));
		if (!memcmp((char*)SDT->Signature, signature, 4)) {
			if (count++ == index) {
				return (uintptr_t)SDT;
			}
		}
	}

	return 0;
}
