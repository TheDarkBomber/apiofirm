#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fsfat.h"
#include "memdefs.h"
#include "memory.h"
#include <stdint.h>

uint8_t* KERNEL_LOAD_BUFFER = (uint8_t *)MEMORY_LOAD_KERNEL;
uint8_t* Bee = (uint8_t*)MEMORY_KERNEL_ADDRESS;

typedef void (*BeeFlight)();

void __attribute__((cdecl)) cstart_(uint16_t bootLocation) {
	print("[CRXBOOT] Loaded second stage bootloader. Bzzzzzt.\r\n");

	clearscreen();
	print("[CRXBOOT] CRXBOOT located on drive %d\r\n", bootLocation);

	DISK disk;
	if (!InitialiseDisk(&disk, bootLocation)) {
		print("[CRXBOOT] Fatal error: Could not initialise disk in drive %d\r\n", bootLocation);
		goto crxboot_ends;
	}

	if (!InitialiseFAT(&disk)) {
		print("[CRXBOOT] Fatal error: Could not initialise FAT.\r\n");
		goto crxboot_ends;
	}

	print("[CRXBOOT] 0x%x = 0o%o = %d !\r\n", 0x0BEE, 0x0BEE, 0x0BEE);
	print("[CRXBOOT] In base 6, 0x%x = %a\r\n", 0x0BEE, 0x0BEE);
	print("[CRXBOOT] In niftimal, that's %A\r\n", 0x0BEE);

	FATFile* fileData = FATOpen(&disk, "/Boot/system.k  ");
	uint32_t read;
	uint8_t* beeBuffer = Bee;
	while ((read = FATRead(&disk, fileData, MEMORY_LOAD_SIZE, KERNEL_LOAD_BUFFER))) {
		memcpy(beeBuffer, KERNEL_LOAD_BUFFER, read);
		beeBuffer += read;
	}
	FATAntiopen(fileData);

	BeeFlight waggleDance = (BeeFlight)Bee;
	waggleDance();

 crxboot_ends:
	for (;;);
}
