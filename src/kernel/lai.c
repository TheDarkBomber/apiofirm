#include "text.h"
#include "panic.h"
#include "memory.h"
#include "acpi.h"
#include "paging.h"
#include "pci.h"
#include <lai/host.h>
#include <acpispec/tables.h>

void laihost_log(int level, const char* msg) {
	printf("%u: %s\n", level, msg);
}

__attribute__((noreturn)) void laihost_panic(const char* msg) {
	GenericKernelPanic("An error occurred interpreting AML: %s", "LAI Error", msg);
	for(;;);
}

void __stack_chk_fail() {
	GenericKernelPanic("Stack check fail.", "LAI Error");
}

void* laihost_scan(const char* signature, uintptr_t index) {
	if (!memcmp((char*)signature, "DSDT", 4)) {
		if (index > 0) return (void *)0;
		acpi_fadt_t* FADT = (acpi_fadt_t*)ACPIGetTableAddress("FACP", 0);
		for (int i = 0; i < sizeof(SystemDescriptorTable); i++)
			MapMemoryV2P((char*)((uint64_t)FADT->dsdt + i), (char*)((uint64_t)FADT->dsdt + i));
		return (void*)(uint64_t)FADT->dsdt;
	}
	return (void*)ACPIGetTableAddress((char*)signature, index);
}

void* laihost_map(uintptr_t address, uintptr_t amount) {
	for (int i = 0; i < amount; i++) MapMemoryV2P((char*)address, (char*)address);
	return (void*)address;
}

void laihost_sleep(uint64_t duration) {}

void laihost_unmap(void* address, uintptr_t amount) {}

uint8_t laihost_pci_readb(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
	if (segment) laihost_panic("PCI segments not supported.");
	return PCIReadByte(bus, device, function, offset);
}

uint16_t laihost_pci_readw(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
  if (segment) laihost_panic("PCI segments not supported.");
  return PCIReadWord(bus, device, function, offset);
}

uint32_t laihost_pci_readd(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
  if (segment) laihost_panic("PCI segments not supported.");
  return PCIReadDword(bus, device, function, offset);
}

void laihost_pci_writeb(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint8_t value) {
  if (segment) laihost_panic("PCI segments not supported.");
  PCIWriteByte(bus, device, function, offset, value);
}

void laihost_pci_writew(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint16_t value) {
  if (segment) laihost_panic("PCI segments not supported.");
  PCIWriteWord(bus, device, function, offset, value);
}

void laihost_pci_writed(uint16_t segment, uint8_t bus, uint8_t device, uint8_t function, uint16_t offset, uint32_t value) {
  if (segment) laihost_panic("PCI segments not supported.");
  PCIWriteDword(bus, device, function, offset, value);
}
