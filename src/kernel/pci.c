#include "pci.h"
#include "paging.h"
#include "text.h"
#include "panic.h"
#include "ahci.h"
#include "memory.h"
#include "x86.h"

PCIRegister* PCIRegistry = (void*)0;
uint64_t PCIRegistryLength = 0;

static void PCI_InitialiseFunction(uint64_t device, uint64_t function) {
	uint64_t addr = device + (function << 12);
	MapMemoryV2P((char*)addr, (char*)addr);
	PCIDevice* pciDevice = (PCIDevice*)addr;

	if (pciDevice->DeviceID == 0 || pciDevice->DeviceID == 0xFFFF) return;
	prints("\x1B[35m[PCI] Discovered device 0x%x: Vendor=0x%x Device=0x%x\n\x1B[0m", addr, pciDevice->VendorID, pciDevice->DeviceID);

	uint32_t deviceType = (pciDevice->Class << 16) | (pciDevice->Subclass << 8) | pciDevice->ProgramInterface;
	switch (deviceType) {
	case 0x010601: // AHCI device
		PCIRegistry = (PCIRegister*)mreallocate((char*)PCIRegistry, ++PCIRegistryLength * sizeof(PCIRegister));
		PCIRegistry[PCIRegistryLength - 1].Type = PCIAHCIDriver;
		PCIRegistry[PCIRegistryLength - 1].Device = pciDevice;
		PCIRegistry[PCIRegistryLength - 1].Driver = (char*)InitialiseAHCIDriver(pciDevice);
	}
}

static void PCI_InitialiseDevice(uint64_t bus, uint64_t device) {
	uint64_t addr = bus + (device << 15);
	MapMemoryV2P((char*)addr, (char*)addr);
	PCIDevice* pciDevice = (PCIDevice*)addr;

	if (pciDevice->DeviceID == 0 || pciDevice->DeviceID == 0xFFFF) return;

	for (uint64_t function = 0; function < 8; function++) PCI_InitialiseFunction(addr, function);
}

static void PCI_InitialiseBus(uint64_t base, uint64_t bus) {
  uint64_t addr = base + (bus << 20);
  MapMemoryV2P((char *)addr, (char *)addr);
  PCIDevice *pciDevice = (PCIDevice *)addr;

  if (pciDevice->DeviceID == 0 || pciDevice->DeviceID == 0xFFFF) return;

  for (uint64_t device = 0; device < 32; device++) PCI_InitialiseDevice(addr, device);
}

void InitialisePCI(MCFGHeader* MCFG) {
	if (!MCFG) GenericKernelPanic("Attempted to initialise PCI but no MCFG was provided.", "MCFG not found");

	unsigned entries = ((MCFG->Header.Length) - sizeof(MCFGHeader)) / sizeof(DeviceCfg);
	for (int i = 0; i < entries; i++) {
		DeviceCfg* cfg = (DeviceCfg*)((uint64_t)MCFG + sizeof(MCFGHeader) + (sizeof(DeviceCfg) * i));
		for (uint64_t bus = cfg->StartBus; bus < cfg->EndBus; bus++) PCI_InitialiseBus(cfg->BaseAddress, bus);
	}
}

uint8_t PCIReadByte(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset) {
	x86OutputDouble(0xCF8, (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFFFC) | 0x80000000);
	return x86Input(0xCFC + (offset & 3));
}

uint16_t PCIReadWord(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset) {
  x86OutputDouble(0xCF8, (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFFFC) | 0x80000000);
  return x86InputWide(0xCFC + (offset & 2));
}

uint32_t PCIReadDword(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset) {
  x86OutputDouble(0xCF8, (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFFFC) | 0x80000000);
  return x86InputDouble(0xCFC);
}

void PCIWriteByte(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset, uint8_t value) {
  x86OutputDouble(0xCF8, (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFFFC) | 0x80000000);
	x86Output(0xCFC + (offset & 3), value);
}

void PCIWriteWord(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset, uint16_t value) {
  x86OutputDouble(0xCF8, (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFFFC) | 0x80000000);
  x86OutputWide(0xCFC + (offset & 2), value);
}

void PCIWriteDword(uint32_t bus, uint32_t slot, uint32_t function, uint16_t offset, uint32_t value) {
  x86OutputDouble(0xCF8, (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFFFC) | 0x80000000);
  x86OutputDouble(0xCFC, value);
}
