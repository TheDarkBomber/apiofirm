#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "stdio.h"
#include "memory.h"
#include "memmap.h"
#include "paging.h"
#include "task.h"
#include "keyboard.h"
#include "disk.h"
#include "floppy.h"
#include "x86.h"
#include "vga.h"
#include "serial.h"
#include "comstdio.h"
#include "fsfat.h"

typedef void (*Function)();

extern uint32_t __bss_start;
extern uint32_t __end;

static Task workerTask;
static Task utask;

Function exeFalse;
extern void userspace();

void UTaskRun() {
	exeFalse();
}

void testing() {};

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

	InitialiseGDT();
	print("[KERNEL] Initialised GDT.\r\n");

	InitialiseIDT();
	print("[KERNEL] Initialised IDT.\r\n");

	__asm__ volatile ("sti");
	if (!InitialiseSerial()) comstrput("[KERNEL] Initialised serial.\r\n");

	uint16_t* memoryRegions = (uint16_t*)0x8000;
	cprint("[KERNEL] Memory regions: 0x%x\r\n", *memoryRegions);

	for (uint8_t i = 0; i < *memoryRegions; i++) {
		MemoryMap* map = (MemoryMap*)(MEMORY_MAP_ORIGIN + i * MEMORY_MAP_LENGTH);
		cprint("[MEMORY MAP] Base address: 0x%x\r\n", map->BaseAddress);
		cprint("[MEMORY MAP] Region length: 0x%x\r\n", map->RegionLength);
		cprint("[MEMORY MAP] Region type: 0x%x\r\n", map->RegionType);
		cprint("[MEMORY MAP] ACPI 3.0 extended attributes: 0b%b\r\n", map->ACPIExtendedAttributes);
	}

	InitialisePaging();
	print("[KERNEL] Initialised paging.\r\n");

	uint8_t* ULTLFPage00 = InitialiseULTLFPage00();
	VGASetFont(ULTLFPage00);
	print("[VGA] Loaded ultlf font.\r\n");

	comstrput("[KEYBOARD] Setting layout.\r\n");
	SetStandardKeymap();
	comstrput("[KEYBOARD] Layout set.\r\n");

	InitialiseMultitasking();
	workerTask.nextTask = &workerTask;
	SetTask(&workerTask);
	CreateTask(&utask, UTaskRun);

	FloppyDetectDrives();
	IRQClearMask(0x00);
	DISK disk;
	IRQClearMask(0x06);
	if (!InitialiseDisk(&disk, bootLocation)) {
		setDefaultColour(VGA_RED | VGA_WHITE << 4);
		print("[DISK] Error.\r\n");
		goto kernel_end;
	}

	if (!InitialiseFAT(&disk)) {
		setDefaultColour(VGA_RED | VGA_WHITE << 4);
		print("[FAT] Failed to initialise FAT.\r\n");
		goto kernel_end;
	}

	FATFile* fileData = FATOpen(&disk, "/exe");
	FATDirectoryEntry entry;

	uint8_t i = 0;
	print("Directory listing for /Exe\r\n");
	setDefaultColour(VGA_LIGHT_RED);
	while (FATReadEntry(&disk, fileData, &entry) && i++ < 3) {
		print("disk%d:/EXE/", bootLocation);
		for (int i = 0; i < 11; i++) charput(entry.Name[i]);
		strput("\r\n");
	}
	FATAntiopen(fileData);
	setDefaultColour(VGA_YELLOW);

	char buffer[100];
	uint32_t read;

	print("Reading file /Bees/apioform.bee\r\n");
	setDefaultColour(VGA_LIGHT_RED);
	fileData = FATOpen(&disk, "bees/apioform.bee");
	while ((read = FATRead(&disk, fileData, sizeof(buffer), buffer))) {
		for (uint32_t i = 0; i < read; i++) {
			if (buffer[i] == '\n') charput('\r');
			charput(buffer[i]);
		}
	}
	FATAntiopen(fileData);
	setDefaultColour(VGA_YELLOW);

	print("Loading file /Exe/false.x\r\n");
	AllocatePage(2, 6);
	uint8_t* loadAddress = (uint8_t*)(PAGE_BLOCK * 2 + PAGE_SIZE * 6);
	uint8_t loadBuffer[4096];
	uint8_t* readBuffer = loadAddress;
	read = 0;
	fileData = FATOpen(&disk, "/Exe/false.x  ");
	while ((read = FATRead(&disk, fileData, sizeof(loadBuffer), loadBuffer))) {
		memcpy(readBuffer, loadBuffer, read);
		readBuffer += read;
	}
	print("Loaded file /Exe/false.x\r\n");
	FATAntiopen(fileData);
	exeFalse = (Function)loadBuffer;

	print("Loading file /bootstub.x86\r\n");
	char newMBR[512];
	read = 0;
	fileData = FATOpen(&disk, "/bootstub.x86");
	while ((read = FATRead(&disk, fileData, sizeof(newMBR), newMBR)));
	FATAntiopen(fileData);
	setDefaultColour(VGA_MAGENTA);
	print("Writing /bootstub.x86 to MBR...\r\n");
	setDefaultColour(VGA_YELLOW);
	if (WriteDiskSectors(&disk, 0, 1, newMBR)) print("Wrote new MBR.\r\n");

 kernel_end:
	for (;;);
}
