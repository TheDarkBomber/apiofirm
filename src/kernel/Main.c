#include <stdint.h>
#include "idt.h"
#include "pic.h"
#include "stdio.h"
#include "memory.h"
#include "memmap.h"
#include "paging.h"
#include "keyboard.h"
#include "x86.h"
#include "serial.h"
#include "comstdio.h"

extern uint32_t __bss_start;
extern uint32_t __end;

void __attribute__((section(".entry"))) bzzzzzt(uint16_t bootLocation) {
	setprintPosition(0, 4);
	print("[KERNEL] Bzzzzzzzzzt! Kernel loaded c:\r\n");
	setDefaultColour(VGA_LIGHT_CYAN);
	print("[KERNEL] In multi-colour.\r\n");
	setDefaultColour(VGA_YELLOW);

	PICRemap(0x20, 0x28);
	IRQMaskAll();
	IRQClearMask(0x01);
	print("[KERNEL] Remapped PIC.\r\n");

	InitialiseIDT();
	print("[KERNEL] Initialised IDT.\r\n");

	__asm__ volatile ("sti");
	if (!InitialiseSerial()) comstrput("[KERNEL] Initialised serial.\r\n");

	uint16_t* memoryRegions = (uint16_t*)0x8000;
	cprint("[KERNEL] Memory regions: 0x%x\r\n", *memoryRegions);

	InitialisePaging();
	print("[KERNEL] Initialised paging.\r\n");
	cprint("[KERNEL] Page directory located at %x\r\n", x86ReadCR3());

	for (uint8_t i = 0; i < *memoryRegions; i++) {
		MemoryMap* map = (MemoryMap*)(MEMORY_MAP_ORIGIN + i * MEMORY_MAP_LENGTH);
		cprint("[MEMORY MAP] Base address: 0x%x\r\n", map->BaseAddress);
		cprint("[MEMORY MAP] Region length: 0x%x\r\n", map->RegionLength);
		cprint("[MEMORY MAP] Region type: 0x%x\r\n", map->RegionType);
		cprint("[MEMORY MAP] ACPI 3.0 extended attributes: 0b%b\r\n", map->ACPIExtendedAttributes);
	}

	comstrput("[KEYBOARD] Setting layout.\r\n");
	SetStandardKeymap();
	comstrput("[KEYBOARD] Layout set.\r\n");
	for(;;);
}
