#ifndef __MEMORY_MAP_KERNEL_H
#define __MEMORY_MAP_KERNEL_H

#include <stdint.h>

#define MEMORY_MAP_REGIONS 0x8000
#define MEMORY_MAP_ORIGIN 0x8004

#define MEMORY_MAP_LENGTH 24

#define MEMORY_MAP_USABLE           1
#define MEMORY_MAP_RSVP             2
#define MEMORY_MAP_ACPI_RECLAIMABLE 3
#define MEMORY_MAP_ACPI_NVS         4
#define MEMORY_MAP_BAD_AREA         5

typedef struct {
	uint64_t BaseAddress;
	uint64_t RegionLength;
	uint32_t RegionType;
	uint32_t ACPIExtendedAttributes;
} __attribute__((packed)) MemoryMap;

#endif
