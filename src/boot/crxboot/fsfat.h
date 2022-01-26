#ifndef __FSFAT_CRXBOOT_H
#define __FSFAT_CRXBOOT_H
#include "stdint.h"
#include "disk.h"

typedef struct {
	uint8_t Name[11];
	uint8_t Attributes;
	uint8_t RSVP;
	uint8_t CreatedDecitime;
	uint16_t CreatedTime;
	uint16_t CreatedDate;
	uint16_t AccessedDate;
	uint16_t FirstClusterHigh;
	uint16_t ModifiedTime;
	uint16_t ModifiedDate;
	uint16_t FirstClusterLow;
	uint32_t Size;
} __attribute__((packed)) FATDirectoryEntry;

typedef struct {
	int Handle;
	bool IsDirectory;
	uint32_t Position;
	uint32_t Size;
} FATFile;

enum FATAttributes {
	READ_ONLY = 0x01,
	HIDDEN = 0x02,
	SYSTEM = 0x04,
	VOLUME_ID = 0x08,
	DIRECTORY = 0x10,
	ARCHIVE = 0x20,
	LFN = READ_ONLY | HIDDEN | SYSTEM | VOLUME_ID
};

bool InitialiseFAT(DISK *disk);
FATFile* FATOpen(DISK *disk, const char *path);
uint32_t FATRead(DISK *disk, FATFile *file, uint32_t byteCount, void *outputData);
bool FATReadEntry(DISK *disk, FATFile *file, FATDirectoryEntry *directoryEntry);
void FATAntiopen(FATFile* file);

#endif
