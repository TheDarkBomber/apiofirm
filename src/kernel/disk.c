#include "disk.h"
#include "floppy.h"
#include "x86.h"
#include "string.h"

extern const char FloppyDMABuffer[0x4800];

bool InitialiseDisk(DISK* disk, uint8_t diskNumber) {
	uint8_t diskType;
	uint16_t cylinders, sectors, heads;

	if (!strcmp(FloppyDetectDrive(), "1.44M")) return false;

	cylinders = 80;
	heads = 2;
	sectors = 18;

	diskType = x86Input(FLOPPY_VERSION);
	ResetFloppy();

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

	for (uint8_t i = 0; i < 3; i++) {
		if (FloppyTrack(cylinder, 0)) {
			outputData = FloppyDMABuffer;
			return true;
		}

		ResetFloppy();
	}

	return false;
}
