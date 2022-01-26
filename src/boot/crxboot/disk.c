#include "disk.h"
#include "x86.h"
#include "stdio.h"

bool InitialiseDisk(DISK* disk, uint8_t diskNumber) {
	uint8_t diskType;
	uint16_t cylinders, sectors, heads;

	if (!x86DiskGetParameters(disk->ID, &diskType, &cylinders, &sectors, &heads))
		return false;

	disk->ID = diskNumber;
	disk->Cylinders = cylinders;
	disk->Heads = heads;
	disk->Sectors = sectors;

	return true;
}

void ConvertLBAToCHS(DISK *disk, uint32_t LBA, uint16_t *outputCylinder, uint16_t *outputSector, uint16_t *outputHead) {
	// Sector = (LBA % Sectors/Track + 1)
	*outputSector = LBA % disk->Sectors + 1;
	// Cylinder = (LBA / Sectors/Track) / Heads
	*outputCylinder = (LBA / disk->Sectors) / disk->Heads;
	// Head = (LBA / Sectors/Track) % Heads
	*outputHead = (LBA / disk->Sectors) % disk->Heads;
}

bool ReadDiskSectors(DISK *disk, uint32_t LBA, uint8_t sectors, void* outputData) {
	uint16_t cylinder, sector, head;
	ConvertLBAToCHS(disk, LBA, &cylinder, &sector, &head);

	for (int i = 0; i < 3; i++) {
		if (x86DiskRead(disk->ID, cylinder, sector, head, sectors, outputData))
			return true;

		x86DiskReset(disk->ID);
	}

	return false;
}
