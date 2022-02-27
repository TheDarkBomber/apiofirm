#ifndef __DISK_KERNEL_H
#define __DISK_KERNEL_H
#include <stdint.h>
#include <stdbool.h>
typedef struct {
	uint8_t ID;
	uint16_t Cylinders;
	uint16_t Sectors;
	uint16_t Heads;
} DISK;

void ConvertLBAToCHS(DISK* disk, uint32_t LBA, uint16_t* outputCylinder, uint16_t* outputSector, uint16_t* outputHead);

bool InitialiseDisk(DISK *disk, uint8_t diskLocation);
bool ReadDiskSectors(DISK *disk, uint32_t LBA, uint8_t sectors, void* outputData);
bool WriteDiskSectors(DISK *disk, uint32_t LBA, uint8_t sectors, void* inputData);

#endif
