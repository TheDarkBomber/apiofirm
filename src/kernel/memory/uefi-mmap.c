#include "uefi-mmap.h"

const char* UefiMemoryMapTypes[] = {
	"Reserved Memory",
	"UEFI Loader Code",
	"UEFI Loader Data",
	"Boot Services Code",
	"Boot Services Data",
	"UEFI Runtime Code",
	"UEFI Runtime Data",
	"Conventional Memory",
	"Unusable Memory",
	"ACPI-Reclaimable Memory",
	"ACPI Memory NVS",
	"Memory Mapped I/O",
	"Memory Mapped I/O Port Space",
	"PAL Code"
};

uint64_t GetTotalMemorySize(UefiMemoryDescriptor* MMap, uint64_t MMapEntries, uint64_t mmapDSize) {
	uint64_t memorySize = 0;

	for (unsigned i = 0; i < MMapEntries; i++) {
		UefiMemoryDescriptor* descriptor = (UefiMemoryDescriptor*)((uint64_t)MMap + (i * mmapDSize));
		memorySize += descriptor->PageCount * 4096;
	}

	return memorySize;
}
