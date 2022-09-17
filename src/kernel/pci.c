#include "pci.h"
#include "paging.h"
#include "text.h"
#include "panic.h"

static void PCI_InitialiseFunction(uint64_t device, uint64_t function) {
	uint64_t addr = device + (function << 12);
	MapMemoryV2P((char*)addr, (char*)addr);
	PCIDevice* pciDevice = (PCIDevice*)addr;

	if (pciDevice->DeviceID == 0 || pciDevice->DeviceID == 0xFFFF) return;
	printf("0x%x: Vendor=0x%x Device=0x%x\n", addr, pciDevice->VendorID, pciDevice->DeviceID);
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
