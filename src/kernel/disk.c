#include "disk.h"
#include "floppy.h"
#include "memory.h"
#include "x86.h"
#include "string.h"
#include "comstdio.h"

extern const char FloppyDMABuffer[0x4800];

static const char *DiskTypeNames[2] = {
	"1.44M Floppy",
	"Virtual RAMDisk"
};

bool InitialiseDisk(DISK* disk, uint8_t diskNumber) {
	uint8_t diskType;
	uint16_t cylinders, sectors, heads;

	if (strcmp(FloppyDetectDrive(), "1.44M") < 1) return false;

	cylinders = 80;
	heads = 2;
	sectors = 18;

	if (x86Input(FLOPPY_VERSION) == 0xFF) diskType = DISK_FLOPPY_1_44M;
	ResetFloppy();

	disk->ID = diskNumber;
	disk->Cylinders = cylinders;
	disk->Heads = heads;
	disk->Sectors = sectors;
	disk->Type = diskType;

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

	switch (disk->Type) {
	case DISK_FLOPPY_1_44M:
		for (uint8_t i = 0; i < 3; i++) {
			if (FloppyTrack(cylinder, head, sector, 0)) {
				uint8_t DataBuffer[512 * sectors];
				for (uint32_t i = 0; i < 512 * sectors; i++) {
					DataBuffer[i] = FloppyDMABuffer[i];
				}
				memcpy(outputData, DataBuffer, 512 * sectors);
				return true;
			}

			ResetFloppy();
		}
		break;
	default:
		cprint("[DISK] Cannot read disk of type 0x%x (%s)\r\n", disk->Type, DiskTypeNames[disk->Type]);
		break;
	}

	return false;
}

bool WriteDiskSectors(DISK *disk, uint32_t LBA, uint8_t sectors, void *inputData) {
	uint16_t cylinder, sector, head;
	ConvertLBAToCHS(disk, LBA, &cylinder, &sector, &head);
	memcpy(FloppyDMABuffer, inputData, 512 * sectors);
	for (uint8_t i = 0; i < 3; i++) {
		if (FloppyTrack(cylinder, head, sector, 1)) {
			return true;
		}
		ResetFloppy();
	}
	return false;
}
