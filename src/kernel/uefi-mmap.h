#ifndef __APIOFIRM_UEFI_MMAP_H_
#define __APIOFIRM_UEFI_MMAP_H_
#include <stdint.h>

typedef struct {
	uint32_t Type;
	void* PhysicalAddress;
	void* VirtualAddress;
	uint64_t PageCount;
	uint64_t Attributes;
} UefiMemoryDescriptor;

typedef enum {
	UefiReservedMemory = 0,
	UefiLoaderCode = 1,
	UefiLoaderData = 2,
	UefiBootServicesCode = 3,
	UefiBootServicesData = 4,
	UefiRuntimeServicesCode = 5,
	UefiRuntimeServicesData = 6,
	UefiConventionalMemory = 7,
	UefiUnusableMemory = 8,
	UefiACPIReclaimableMemory = 9,
	UefiACPIMemoryNVS = 10,
	UefiMemoryMappedIO = 11,
	UefiMemoryMappedIOPortSpace = 12,
	UefiPALCode = 13
} UefiMemoryMapType;

uint64_t GetTotalMemorySize(UefiMemoryDescriptor* MMap, uint64_t MMapEntries, uint64_t mmapDSize);

extern const char* UefiMemoryMapTypes[];

#endif
