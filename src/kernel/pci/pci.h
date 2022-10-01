#ifndef __APIOFIRM_PCI_H_
#define __APIOFIRM_PCI_H_
#include <acpi/acpi.h>

typedef enum {
	PCINullDriver = 0,
	PCIAHCIDriver = 1
} PCIDriverType;

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
} __attribute__((packed)) PCIDevice;

typedef struct {
	PCIDevice Device;
	uint32_t BAR0;
	uint32_t BAR1;
	uint32_t BAR2;
	uint32_t BAR3;
	uint32_t BAR4;
	uint32_t BAR5;
	uint32_t Cardbus;
	uint16_t SubsystemVendorID;
	uint16_t SubsystemID;
	uint32_t ExpansionROMAddress;
	uint8_t Capabilities;
	uint8_t RSVP[56];
	uint8_t InterruptLine;
	uint8_t InterruptPin;
	uint8_t MinimumGrant;
	uint8_t MaximumLatency;
} __attribute__((packed)) PCIHeader0;

typedef struct {
	PCIDriverType Type;
	PCIDevice* Device;
	char* Driver;
} PCIRegister;

void InitialisePCI(MCFGHeader* MCFG);
extern PCIRegister* PCIRegistry;
extern uint64_t PCIRegistryLength;

uint8_t PCIReadByte(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset);
uint16_t PCIReadWord(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset);
uint32_t PCIReadDword(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset);

void PCIWriteByte(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset, uint8_t value);
void PCIWriteWord(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset, uint16_t value);
void PCIWriteDword(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset, uint32_t value);

#endif
