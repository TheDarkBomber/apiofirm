#include "ahci.h"
#include "disk.h"
#include "memory.h"
#include "paging.h"
#include "text.h"
#include "kattrs.h"

const char* AHCIPortTypeStrings[] = {
	"Null",
	"SATA",
	"SEMB",
	"PM",
	"SATAPI"
};

AHCIDriver* InitialiseAHCIDriver(PCIDevice* device) {
	AHCIDriver* driver = (AHCIDriver*)mallocate(sizeof(AHCIDriver));
	driver->PortAmount = 0;
	driver->PCIBase = device;
	((PCIHeader0*)device)->InterruptLine = 3;
	driver->ABAR = (AHCIMemoryHBA*)(uintptr_t)((PCIHeader0*)device)->BAR5;
	MapMemoryV2P((char*)driver->ABAR, (char*)driver->ABAR);
	AHCIClearInterrupts(driver);
	/* printf("[GHC] 0b%b\n", driver->ABAR->GlobalHostControl); */
	driver->ABAR->GlobalHostControl |= (1 << 1); // Enable interrupts
	/* printf("[GHC] 0b%b\n", driver->ABAR->GlobalHostControl); */
	prints("\x1B[1;32m[AHCI] Initialised AHCI driver 0x%X\n\x1B[0m", driver->PCIBase);
	ProbeAHCIPorts(driver);

	for (int i = 0; i < driver->PortAmount; i++) {
		ConfigureAHCIPort(driver->Ports[i]);
		prints("\x1B[34m[AHCI] Configured port %u of type %s\n\x1B[0m", driver->Ports[i]->Number, AHCIPortTypeStrings[driver->Ports[i]->Type]);

		driver->Ports[i]->Buffer = RequestPages(16);
		driver->Ports[i]->PagesAllocated = 16;
	}

	return driver;
}

void AHCIClearInterrupts(AHCIDriver* driver) {
	driver->ABAR->GlobalHostControl &= ~(1 << 1);
	for (int i = 0; i < 32; i++) {
		if ((driver->ABAR->ImplementedPorts & (1 << i)) == 0) continue;
		driver->ABAR->Ports[i].InterruptEnable = 0;
		driver->ABAR->Ports[i].InterruptStatus = ~0;
	}
	driver->ABAR->InterruptStatus = ~0;
}

void ProbeAHCIPorts(AHCIDriver* driver) {
	AHCIPort** temporaryPortStore = (AHCIPort**)mallocate(32 * sizeof(AHCIPort*));
	for (int i = 0; i < 32; i++) {
		if (driver->ABAR->ImplementedPorts & (1 << i)) {
			AHCIPortType type = AHCICheckPortType(&driver->ABAR->Ports[i]);
			prints("\x1B[3;34m[AHCI] Detected %s port.\n\x1B[0m", AHCIPortTypeStrings[type]);
			if (type == AHCISATAPort || type == AHCISATAPIPort) {
				temporaryPortStore[driver->PortAmount] = (AHCIPort*)mallocate(sizeof(AHCIPort));
				temporaryPortStore[driver->PortAmount]->Type = type;
				temporaryPortStore[driver->PortAmount]->HBA = &driver->ABAR->Ports[i];
				temporaryPortStore[driver->PortAmount]->Number = driver->PortAmount;
				driver->PortAmount++;
			}
		}
	}

	driver->Ports = (AHCIPort**)mallocate(driver->PortAmount * sizeof(AHCIPort*));
	memcpy((char*)driver->Ports, (char*)temporaryPortStore, sizeof(AHCIPort*) * driver->PortAmount);
	mfree((char*)temporaryPortStore);
}

AHCIPortType AHCICheckPortType(AHCIPortHBA* port) {
	uint8_t IPM = (port->SATAStatus >> 8) & 0b111;
	uint8_t DeviceDetection = port->SATAStatus & 0b111;

	if (DeviceDetection != AHCI_HBA_PORT_DEVICE_PRESENT) return AHCINullPort;
	if (IPM != AHCI_HBA_PORT_IPM_ACTIVE) return AHCINullPort;

	switch (port->Signature) {
	case AHCI_SATA_ATAPI_SIGNATURE: return AHCISATAPIPort;
	case AHCI_SATA_ATA_SIGNATURE: return AHCISATAPort;
	case AHCI_SATA_PM_SIGNATURE: return AHCIPMPort;
	case AHCI_SATA_SEMB_SIGNATURE: return AHCIPMPort;
	default: return AHCINullPort;
	}
}

void ConfigureAHCIPort(AHCIPort* port) {
	StopAHCICommandEngine(port);
	char* newCLB = RequestPage();
	port->HBA->CommandListBase = (uint64_t)newCLB;
	memset((char*)port->HBA->CommandListBase, 0, 1024);
	char* newFIS = RequestPage();
	port->HBA->FISAddress = (uint64_t)newFIS;
	memset(newFIS, 0, 256);
	port->HBA->SATAError = ~0;
	port->HBA->InterruptStatus = ~0;
	port->HBA->InterruptEnable = 1;

	AHCICommandHBA* command = (AHCICommandHBA*)(port->HBA->CommandListBase);
	for (int i = 0; i < 32; i++) {
		command[i].PRDTLength = 8;
		char* CommandTableAddress = RequestPage();
		uint64_t addr = (uint64_t)CommandTableAddress + (i << 8);
		command[i].CommandTableBase = addr;
		memset(CommandTableAddress, 0, 256);
	}

	StartAHCICommandEngine(port);
}

void StopAHCICommandEngine(AHCIPort* port) {
	port->HBA->CommandStatus &= ~AHCI_HBA_PxCMD_ST;
	port->HBA->CommandStatus &= ~AHCI_HBA_PxCMD_FRE;

	for (;;) {
		if (port->HBA->CommandStatus & AHCI_HBA_PxCMD_FR) continue;
		if (port->HBA->CommandStatus & AHCI_HBA_PxCMD_CR) continue;
		break;
	}
}

void StartAHCICommandEngine(AHCIPort* port) {
	while (port->HBA->CommandStatus & AHCI_HBA_PxCMD_CR);
	port->HBA->CommandStatus |= AHCI_HBA_PxCMD_FRE;
	port->HBA->CommandStatus |= AHCI_HBA_PxCMD_ST;
}

static int FindCommandSlot(AHCIPort* port) {
	uint32_t slots = (port->HBA->SATAActive | port->HBA->CommandIssue);
	for (int i = 0; i < 32; i++) {
		if ((slots & 1) == 0) return i;
		slots = slots >> 1;
	}
	printf("[AHCI] No slots available.\n");
	return -1;
}

bool ReadWriteAHCIPort(AHCIPort* port, uint64_t sector, uint32_t size, bool write) {
	uint32_t lowerSector = (uint32_t)sector;
	uint32_t upperSector = (uint32_t)(sector >> 32);
	port->HBA->InterruptStatus = (uint32_t)-1;
	int slot = FindCommandSlot(port);
	if (slot == -1) return false;
	AHCICommandHBA* command = (AHCICommandHBA*)port->HBA->CommandListBase;
	command += slot;
	command->CommandFISLength = sizeof(AHCIRegisterFISH2D) / sizeof(uint32_t);
	command->Write = write;
	command->PRDTLength = 1;

	AHCICommandTableHBA* commandTable = (AHCICommandTableHBA*)(command->CommandTableBase);
	memset((char*)commandTable, 0, sizeof(AHCICommandTableHBA) + (command->PRDTLength - 1) * sizeof(AHCIPRDTEntryHBA));

	int i;
	char* buffer = port->Buffer;

	for (i = 0; i < command->PRDTLength - 1; i++) {
		commandTable->PRDT[i].Data = (uint64_t)buffer;
		commandTable->PRDT[i].ByteAmount = 8 * 1024 - 1;
		commandTable->PRDT[i].InterruptOnCompletion = 1;
		buffer += 4 * 1024;
		size -= 16;
	}

	commandTable->PRDT[i].Data = (uint64_t)port->Buffer;
	commandTable->PRDT[i].ByteAmount = (size << 9) - 1;
	commandTable->PRDT[i].InterruptOnCompletion = 1;

	AHCIRegisterFISH2D* FIS = (AHCIRegisterFISH2D*)(&commandTable->FIS);
	FIS->Type = FIS_REGISTER_H2D;
	FIS->CommandControl = 1;
	FIS->Command = write ? AHCI_ATA_COMMAND_WRITE_DMA_EX : AHCI_ATA_COMMAND_READ_DMA_EX;

	FIS->LBA0 = (uint8_t)lowerSector;
	FIS->LBA1 = (uint8_t)(lowerSector >> 8);
	FIS->LBA2 = (uint8_t)(lowerSector >> 16);
	FIS->LBA3 = (uint8_t)upperSector;
	FIS->LBA4 = (uint8_t)(upperSector >> 8);
	FIS->LBA5 = (uint8_t)(upperSector >> 16);

	FIS->DeviceRegister = 1 << 6;
	FIS->LowerCount = size & 0xFF;
	FIS->UpperCount = (size >> 8) & 0xFF;

	uint64_t spinlock = 0;
	while ((port->HBA->TaskFileData & (AHCI_ATA_DEVICE_BUSY | AHCI_ATA_DEVICE_DRQ)) && spinlock < 1000000) spinlock++;
	if (spinlock >= 1000000) return false;

	port->HBA->CommandIssue = 1 << slot;

	for (;;) {
		if ((port->HBA->CommandIssue & (1 << slot)) == 0) break;
		if (port->HBA->InterruptStatus & AHCI_HBA_PxIS_TFES) return false;
	}

	return true;
}

// DISK API

bool AHCIDiskAPI_ReadSector(AHCIPort* driver, uint64_t LBA, char* data) {
	if (!ReadWriteAHCIPort(driver, LBA, 1, false)) return false;
	memcpy(data, driver->Buffer, 512);
	return true;
}

bool AHCIDiskAPI_WriteSector(AHCIPort* driver, uint64_t LBA, char* data) {
	memcpy(driver->Buffer, data, 512);
	bool success = ReadWriteAHCIPort(driver, LBA, 1, true);
	return success;
}

bool AHCIDiskAPI_ReadSectors(AHCIPort* driver, uint64_t LBA, uint64_t amount, char* data) {
	uint64_t SectorsPerPBuffer = (driver->PagesAllocated * 4096) / 512;
	uint64_t Cycles = amount / SectorsPerPBuffer;
	uint64_t Take = amount % SectorsPerPBuffer;
	int i = 0;
	for (; Cycles > 0; Cycles--, i++) {
		if (!ReadWriteAHCIPort(driver, LBA + (SectorsPerPBuffer * i), SectorsPerPBuffer, false)) return false;
		memcpy(data + (512 * SectorsPerPBuffer * i), driver->Buffer, 512 * SectorsPerPBuffer);
	}

	if (!ReadWriteAHCIPort(driver, LBA + (SectorsPerPBuffer * i), Take, false)) return false;
	memcpy(data + (512 * SectorsPerPBuffer * i), driver->Buffer, 512 * Take);
	return true;
}

bool AHCIDiskAPI_WriteSectors(AHCIPort* driver, uint64_t LBA, uint64_t amount, char* data) {
	uint64_t SectorsPerPBuffer = (driver->PagesAllocated * 4096) / 512;
	uint64_t Cycles = amount / SectorsPerPBuffer;
	uint64_t Take = amount % SectorsPerPBuffer;
	int i = 0;
	for (; Cycles > 0; Cycles--, i++) {
		memcpy(driver->Buffer, data + (512 * SectorsPerPBuffer * i), 512 * SectorsPerPBuffer);
		ApiofirmDisableEOI();
		if (!ReadWriteAHCIPort(driver, LBA + (SectorsPerPBuffer * i), SectorsPerPBuffer, true)) return false;
		ApiofirmEnableEOI();
	}

	memcpy(driver->Buffer, data + (512 * SectorsPerPBuffer * i), 512 * SectorsPerPBuffer);
	bool success = ReadWriteAHCIPort(driver, LBA + (SectorsPerPBuffer * i), Take, true);
	return success;
}

DiskAPI AHCIDiskAPI = {
	.ReadSingle = (DiskAPI_ReadSector)AHCIDiskAPI_ReadSector,
	.Read = (DiskAPI_ReadSectors)AHCIDiskAPI_ReadSectors,
	.WriteSingle = (DiskAPI_WriteSector)AHCIDiskAPI_WriteSector,
	.Write = (DiskAPI_WriteSectors)AHCIDiskAPI_WriteSectors
};
