#include "stdint.h"
#include "stdio.h"
#include "disk.h"
#include "fsfat.h"

void far *Data = (void far *)0x00500200;

void _cdecl cstart_(uint16_t bootLocation) {
	strput("[CRXBOOT] Loaded second stage bootloader. Bzzzzzt.\r\n");
	print("[CRXBOOT] CRXBOOT located on drive %d\r\n", bootLocation);

	DISK disk;
	if (!InitialiseDisk(&disk, bootLocation)) {
		print("[CRXBOOT] Fatal error: Could not initialise disk in drive %d\r\n", bootLocation);
		goto crxboot_ends;
	}

	ReadDiskSectors(&disk, 19, 1, Data);

	if (!InitialiseFAT(&disk)) {
		print("[CRXBOOT] Fatal error: Could not initialise FAT.\r\n");
		goto crxboot_ends;
	}
	print("[CRXBOOT] 0x%x = 0o%o = %d !\r\n", 0x0BEE, 0x0BEE, 0x0BEE);
	print("[CRXBOOT] In base 6, 0x%x = %a\r\n", 0x0BEE, 0x0BEE);
	print("[CRXBOOT] In niftimal, that's %A\r\n", 0x0BEE);

	print("[CRXBOOT] Directory listing of first five files for /\r\n");
	FATFile far* fileData = FATOpen(&disk, "/");
	FATDirectoryEntry entry;

	int i = 0;
	FATReadEntry(&disk, fileData, &entry);
	while (FATReadEntry(&disk, fileData, &entry) && i++ < 5) {
		print("disk%d:/", bootLocation);
		for (int i = 0; i < 11; i++) charput(entry.Name[i]);
		strput("\r\n");
	}
	FATAntiopen(fileData);

	print("[CRXBOOT] Directory listing of first three files for /BEES\r\n");
	fileData = FATOpen(&disk, "/bees");
	i = 0;
	while (FATReadEntry(&disk, fileData, &entry) && i++ < 3) {
		print("disk%d:/BEES/", bootLocation);
		for (int i = 0; i < 11; i++) charput(entry.Name[i]);
		strput("\r\n");
	}
	FATAntiopen(fileData);

	char buffer[100];
	uint32_t read;

	print("[CRXBOOT] Performing standard read of /BEES/APIOFORM.BEE\r\n");
	fileData = FATOpen(&disk, "bees/apioform.bee");
	while ((read = FATRead(&disk, fileData, sizeof(buffer), buffer))) {
		for (uint32_t i = 0; i < read; i++) {
			if (buffer[i] == '\n') charput('\r');
			charput(buffer[i]);
		}
	}
	FATAntiopen(fileData);

 crxboot_ends:
	for (;;);
}
