#ifndef __APIOFIRM_ACPI_H_
#define __APIOFIRM_ACPI_H_
#include <stdint.h>

typedef struct {
	uint8_t Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	uint8_t OEMId[6];
	uint8_t OEMTableId[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
} __attribute__((packed)) SystemDescriptorTable;

typedef struct {
	SystemDescriptorTable Header;
	uint64_t RSVP;
} __attribute__((packed)) MCFGHeader;

typedef struct {
	uint64_t BaseAddress;
	uint16_t PCISegmentGroup;
	uint8_t StartBus;
	uint8_t EndBus;
	uint32_t RSVP;
} __attribute__((packed)) DeviceCfg;

SystemDescriptorTable* ACPIFindTable(SystemDescriptorTable* SDT, char* signature);
void ACPISetXSDT(SystemDescriptorTable* XSDT);
uintptr_t ACPIGetTableAddress(char* signature, uintptr_t index);

#endif
