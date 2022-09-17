#ifndef __APIOFIRM_PCI_H_
#define __APIOFIRM_PCI_H_
#include "acpi.h"

typedef struct {
	uint16_t VendorID;
	uint16_t DeviceID;
	uint16_t Command;
	uint16_t Status;
	uint8_t RevisionID;
	uint8_t ProgramInterface;
	uint8_t Subclass;
	uint8_t Class;
	uint8_t CacheLineSize;
	uint8_t LatencyTimer;
	uint8_t HeaderType;
	uint8_t BIST;
} PCIDevice;

void InitialisePCI(MCFGHeader* MCFG);

#endif
