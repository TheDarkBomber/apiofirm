#ifndef __APIOFIRM_AHCI_H_
#define __APIOFIRM_AHCI_H_
#include "pci.h"
#include <stdint.h>
#include <stdbool.h>

#define AHCI_HBA_PORT_DEVICE_PRESENT 0x03
#define AHCI_HBA_PORT_IPM_ACTIVE 0x01
#define AHCI_SATA_ATAPI_SIGNATURE 0xEB140101
#define AHCI_SATA_ATA_SIGNATURE 0x00000101
#define AHCI_SATA_SEMB_SIGNATURE 0xC33C0101
#define AHCI_SATA_PM_SIGNATURE 0x96690101

#define AHCI_HBA_PxCMD_CR 0x8000
#define AHCI_HBA_PxCMD_FRE 0x0010
#define AHCI_HBA_PxCMD_ST 0x0001
#define AHCI_HBA_PxCMD_FR 0x4000

#define AHCI_ATA_DEVICE_BUSY 0x80
#define AHCI_ATA_DEVICE_DRQ 0x08
#define AHCI_ATA_COMMAND_READ_DMA_EX 0x25
#define AHCI_ATA_COMMAND_READ_SECTORS_EX 0x24
#define AHCI_ATA_COMMAND_WRITE_DMA_EX 0x35

#define AHCI_HBA_PxIS_TFES (1 << 30)

typedef enum {
	AHCINullPort = 0,
	AHCISATAPort = 1,
	AHCISEMBPort = 2,
	AHCIPMPort = 3,
	AHCISATAPIPort = 4
} AHCIPortType;

typedef enum {
	FIS_REGISTER_H2D = 0x27,
	FIS_REGISTER_D2H = 0x34,
	FIS_DMA_ACTIVE = 0x39,
	FIS_DMA_SETUP = 0x41,
	FIS_DATA = 0x46,
	FIS_BIST = 0x58,
	FIS_PIO_SETUP = 0x5F,
	FIS_DEVICE_BITS = 0xA1
} AHCIFISType;

typedef struct {
	uint64_t CommandListBase;
	uint64_t FISAddress;
	uint32_t InterruptStatus;
	uint32_t InterruptEnable;
	uint32_t CommandStatus;
	uint32_t RSVP0;
	uint32_t TaskFileData;
	uint32_t Signature;
	uint32_t SATAStatus;
	uint32_t SATAControl;
	uint32_t SATAError;
	uint32_t SATAActive;
	uint32_t CommandIssue;
	uint32_t SataNotification;
	uint32_t FISSwitchControl;
	uint32_t RSVP1[11];
	uint32_t Vendor[4];
} __attribute__((packed)) AHCIPortHBA;

typedef struct {
	uint32_t HostCapability;
	uint32_t GlobalHostControl;
	uint32_t InterruptStatus;
	uint32_t ImplementedPorts;
	uint32_t Version;
	uint32_t CCCControl;
	uint32_t CCCPorts;
	uint32_t EnclosureManagementLocation;
	uint32_t EnclosureManagementControl;
	uint32_t ExtendedHostCapabilities;
	uint32_t BIOSHandoffControlStatus;
	uint8_t RSVP[0x74];
	uint8_t Vendor[0x60];
	AHCIPortHBA Ports[1];
} __attribute__((packed)) AHCIMemoryHBA;

typedef struct {
	uint8_t CommandFISLength : 5;
	uint8_t ATAPI : 1;
	uint8_t Write : 1;
	uint8_t Prefetchable : 1;
	uint8_t Reset : 1;
	uint8_t BIST : 1;
	uint8_t ClearBusy : 1;
	uint8_t RSVP0 : 1;
	uint8_t PortMultiplier : 4;
	uint16_t PRDTLength;
	uint32_t PRDBCount;
	uint64_t CommandTableBase;
	uint32_t RSVP1[4];
} __attribute__((packed)) AHCICommandHBA;

typedef struct {
	uint64_t Data;
	uint32_t RSVP0;
	uint32_t ByteAmount : 22;
	uint32_t RSVP1 : 9;
	uint32_t InterruptOnCompletion : 1;
} __attribute__((packed)) AHCIPRDTEntryHBA;

typedef struct {
	uint8_t FIS[64];
	uint8_t ATAPI[16];
	uint8_t RSVP[48];
	AHCIPRDTEntryHBA PRDT[];
} __attribute__((packed)) AHCICommandTableHBA;

typedef struct {
	uint8_t Type;
	uint8_t PortMultiplier : 4;
	uint8_t RSVP0 : 3;
	uint8_t CommandControl : 1;
	uint8_t Command;
	uint8_t LowerFeature;
	uint8_t LBA0;
	uint8_t LBA1;
	uint8_t LBA2;
	uint8_t DeviceRegister;
	uint8_t LBA3;
	uint8_t LBA4;
	uint8_t LBA5;
	uint8_t UpperFeature;
	uint8_t LowerCount;
	uint8_t UpperCount;
	uint8_t CommandCompletion;
	uint8_t Control;
	uint8_t RSVP1[4];
} __attribute__((packed)) AHCIRegisterFISH2D;

typedef struct {
	AHCIPortHBA* HBA;
	char* Buffer;
	uint8_t Number;
	uint8_t PagesAllocated;
	AHCIPortType Type;
} AHCIPort;

typedef struct {
	PCIDevice* PCIBase;
	AHCIMemoryHBA* ABAR;
	AHCIPort** Ports;
	uint8_t PortAmount;
} AHCIDriver;

AHCIDriver* InitialiseAHCIDriver(PCIDevice* device);
void ProbeAHCIPorts(AHCIDriver* driver);
AHCIPortType AHCICheckPortType(AHCIPortHBA* port);

void ConfigureAHCIPort(AHCIPort* port);
void StartAHCICommandEngine(AHCIPort* port);
void StopAHCICommandEngine(AHCIPort* port);

bool ReadWriteAHCIPort(AHCIPort* port, uint64_t sector, uint32_t size, bool write);

// DISK API

bool AHCIDiskAPI_ReadSector(AHCIPort* driver, uint64_t LBA, char* data);
bool AHCIDiskAPI_WriteSector(AHCIPort* driver, uint64_t LBA, char* data);
bool AHCIDiskAPI_ReadSectors(AHCIPort* driver, uint64_t LBA, uint64_t amount, char* data);
bool AHCIDiskAPI_WriteSectors(AHCIPort* driver, uint64_t LBA, uint64_t amount, char* data);

#endif
