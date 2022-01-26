#include "fsfat.h"
#include "disk.h"
#include "stdio.h"
#include "memdefs.h"
#include "string.h"
#include "memory.h"
#include "ctype.h"
#include "stdmacros.h"
#include <stddef.h>

#define SECTOR_SIZE 512
#define MAXIMUM_PATH_SIZE 256
#define MAXIMUM_FILE_HANDLES 10
#define ROOT_HANDLE -1

typedef struct {
	uint8_t BootJMP[3];
	uint8_t OEM[8];
	uint16_t BytesPSector;
	uint8_t SectorsPCluster;
	uint16_t RSVPSectors;
	uint8_t FATCount;
	uint16_t DirectoryEntryCount;
	uint16_t TotalSectors;
	uint8_t MDT;
	uint16_t SectorsPFAT;
	uint16_t SectorsPTrack;
	uint16_t Heads;
	uint32_t HiddenSectors;
	uint32_t LargeSectorCount;

	// EBR
	uint8_t DiskNumber;
	uint8_t _RSVP;
	uint8_t Signature;
	uint32_t VolumeID;
	uint8_t VolumeLabel[11];
	uint8_t SystemID[8];
} __attribute__((packed))FATBoot;

typedef struct {
	uint8_t Buffer[SECTOR_SIZE];
	FATFile Public;
	bool Opened;
	uint32_t FirstCluster;
	uint32_t CurrentCluster;
	uint32_t CurrentSectorInCluster;
} FATFileData;

typedef struct {
	union {
		FATBoot Boot;
		uint8_t BootSectorBytes[SECTOR_SIZE];
	} BootSector;

	FATFileData RootDirectory;
	FATFileData OpenedFiles[MAXIMUM_FILE_HANDLES];
} FATData;

static FATData* Data;
static uint8_t* FileAllocationTable = null;
static uint32_t DataSectionLBA;

bool FATReadBootSector(DISK* disk) {
	return ReadDiskSectors(disk, 0, 1, Data->BootSector.BootSectorBytes);
}

bool FATReadTable(DISK* disk) {
	return ReadDiskSectors(disk, Data->BootSector.Boot.RSVPSectors, Data->BootSector.Boot.SectorsPFAT, FileAllocationTable);
}

bool InitialiseFAT(DISK* disk) {
	Data = (FATData *)MEMORY_FAT_ADDRESS;

	if (!FATReadBootSector(disk)) {
		strput("[FAT ERROR] Boot sector read failure.\r\n");
		return false;
	}

	FileAllocationTable = (uint8_t *)Data + sizeof(FATData);
	uint32_t FATSize = Data->BootSector.Boot.BytesPSector * Data->BootSector.Boot.SectorsPFAT;

	if (sizeof(FATData) + FATSize >= MEMORY_FAT_SIZE) {
		print("[FAT ERROR] Not enough memory to read FAT. Requires %lu, but there is only %u\r\n", sizeof(FATData) + FATSize, MEMORY_FAT_SIZE);
		return false;
	}

	if (!FATReadTable(disk)) {
		print("[FAT ERROR] FAT Read failure.\r\n");
		return false;
	}

	uint32_t rootLBA = Data->BootSector.Boot.RSVPSectors + Data->BootSector.Boot.SectorsPFAT * Data->BootSector.Boot.FATCount;
	uint32_t rootSize = sizeof(FATDirectoryEntry) * Data->BootSector.Boot.DirectoryEntryCount;

	Data->RootDirectory.Public.Handle = ROOT_HANDLE;
	Data->RootDirectory.Public.IsDirectory = true;
	Data->RootDirectory.Public.Position = 0;
	Data->RootDirectory.Public.Size = sizeof(FATDirectoryEntry) * Data->BootSector.Boot.DirectoryEntryCount;
	Data->RootDirectory.Opened = true;
	Data->RootDirectory.FirstCluster = rootLBA;
	Data->RootDirectory.CurrentCluster = rootLBA;
	Data->RootDirectory.CurrentSectorInCluster = 0;

	if (!ReadDiskSectors(disk, rootLBA, 1, Data->RootDirectory.Buffer)) {
		print("[FAT ERROR] Root directory,,, could not be read.\r\n");
		return false;
	}

	uint32_t rootSectors = (rootSize + Data->BootSector.Boot.BytesPSector - 1) / Data->BootSector.Boot.BytesPSector;
	DataSectionLBA = rootLBA + rootSectors;

	for (int i = 0; i < MAXIMUM_FILE_HANDLES; i++) Data->OpenedFiles[i].Opened = false;

	return true;
}

uint32_t FATClusterToLBA(uint32_t cluster) {
	return DataSectionLBA + (cluster - 2) * Data->BootSector.Boot.SectorsPCluster;
}

FATFile* FATOpenEntry(DISK* disk, FATDirectoryEntry* entry) {
	int handle = -1;
	for (int i = 0; i < MAXIMUM_FILE_HANDLES && handle < 0; i++) {
		if (!Data->OpenedFiles[i].Opened) handle = i;
	}

	if (handle < 0) {
		print("[FAT ERROR] No more file handles :c\r\n");
		return false;
	}

	FATFileData* fileData = &Data->OpenedFiles[handle];
	fileData->Public.Handle = handle;
	fileData->Public.IsDirectory = (entry->Attributes & DIRECTORY) != 0;
	fileData->Public.Position = 0;
	fileData->Public.Size = entry->Size;
	fileData->FirstCluster = entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
	fileData->CurrentCluster = fileData->FirstCluster;
	fileData->CurrentSectorInCluster=0;

	if (!ReadDiskSectors(disk, FATClusterToLBA(fileData->CurrentCluster), 1, fileData->Buffer)) {
		print("[FAT ERROR] Skill issue.\r\n");
		print("[FAT ERROR] Opening of entry failed, read error cluster=%u; LBA=%u\r\n", fileData->CurrentCluster, FATClusterToLBA(fileData->CurrentCluster));
		for (int i = 0; i < 11; i++) print("%c", entry->Name[i]);
		print("\r\n");
		return false;
	}

	fileData->Opened = true;
	return &fileData->Public;
}

uint32_t FATNextCluster(uint32_t currentCluster) {
	uint32_t FATIndex = currentCluster * 3 / 2;
	if (currentCluster % 2 == 0) return (*(uint16_t *)(FileAllocationTable + FATIndex)) & 0x0FFF;
	return (*(uint16_t *)(FileAllocationTable + FATIndex)) >> 4;
}

uint32_t FATRead(DISK* disk, FATFile* file, uint32_t byteCount, void* outputData) {
	FATFileData* fileData = (file->Handle == ROOT_HANDLE)
		? &Data->RootDirectory
		: &Data->OpenedFiles[file->Handle];

	uint8_t* unsigned8outputData = (uint8_t*)outputData;

	if (!fileData->Public.IsDirectory || (fileData->Public.IsDirectory && fileData->Public.Size != 0))
		byteCount = MIN(byteCount, fileData->Public.Size - fileData->Public.Position);

	while (byteCount > 0) {
		uint32_t leftInBuffer = SECTOR_SIZE - (fileData->Public.Position % SECTOR_SIZE);
		uint32_t take = MIN(byteCount, leftInBuffer);

		memcpy(unsigned8outputData, fileData->Buffer + fileData->Public.Position % SECTOR_SIZE, take);
		unsigned8outputData += take;
		fileData->Public.Position += take;
		byteCount -= take;

		if (leftInBuffer == take) {
			if (fileData->Public.Handle == ROOT_HANDLE) {
				++fileData->CurrentCluster;

				if (!ReadDiskSectors(disk, fileData->CurrentCluster, 1, fileData->Buffer)) {
					print("[FAT ERROR] Skill issue :c\r\n");
					break;
				}
			} else {
				if (++fileData->CurrentSectorInCluster >= Data->BootSector.Boot.SectorsPCluster) {
					fileData->CurrentSectorInCluster = 0;
					fileData->CurrentCluster = FATNextCluster(fileData->CurrentCluster);
				}

				if (fileData->CurrentCluster >= 0xFF8) {
					fileData->Public.Size = fileData->Public.Position;
					break;
				}

				if (!ReadDiskSectors(disk, FATClusterToLBA(fileData->CurrentCluster) + fileData->CurrentSectorInCluster, 1, fileData->Buffer)) {
					print("[FAT ERROR] Issue with skill.\r\n");
					break;
				}
			}
		}
	}

	return unsigned8outputData - (uint8_t*)outputData;
}

bool FATReadEntry(DISK* disk, FATFile* file, FATDirectoryEntry* directoryEntry) {
	return FATRead(disk, file, sizeof(FATDirectoryEntry), directoryEntry) == sizeof(FATDirectoryEntry);
}

void FATAntiopen(FATFile* file) {
	if (file->Handle == ROOT_HANDLE) {
		file->Position = 0;
		Data->RootDirectory.CurrentCluster = Data->RootDirectory.FirstCluster;
	} else Data->OpenedFiles[file->Handle].Opened = false;
}

bool FATFindFile(DISK* disk, FATFile* file, const char* name, FATDirectoryEntry* outputEntry) {
	char FATName[12];
	FATDirectoryEntry entry;

	memset(FATName, ' ', sizeof(FATName));
	FATName[11] = '\0';

	const char* extension = strchr(name, '.');
	if (extension == null) extension = name + 11;

	for (int i = 0; i < 8 && name[i] && name + i < extension; i++)
		FATName[i] = upcase(name[i]);

	if (extension != name + 11) {
		for (int i = 0; i < 3 && extension[i + 1]; i++)
			FATName[i + 8] = upcase(extension[i + 1]);
	}

	while (FATReadEntry(disk, file, &entry)) {
		if (memcmp(FATName, entry.Name, 11) == 0) {
			*outputEntry = entry;
			return true;
		}
	}

	return false;
}

FATFile* FATOpen(DISK* disk, const char* path) {
	char name[MAXIMUM_PATH_SIZE];

	if (path[0] == '/') path++;

	FATFile* current = &Data->RootDirectory.Public;

	while (*path) {
		bool isLast = false;
		const char* delimiter = strchr(path, '/');
		if (delimiter != null) {
			memcpy(name, path, delimiter - path);
			name[delimiter - path + 1] = '\0';
			path = delimiter + 1;
		} else {
			unsigned length = strlen(path);
			memcpy(name, path, length);
			name[length + 1] = '\0';
			path += length;
			isLast = true;
		}

		FATDirectoryEntry entry;
		if (FATFindFile(disk, current, name, &entry)) {
			FATAntiopen(current);

			if (!isLast && entry.Attributes & DIRECTORY == 0) {
				print("[FAT ERROR] %s is NOT a directory.\r\n", name);
				return null;
			}

			current = FATOpenEntry(disk, &entry);
		} else {
			FATAntiopen(current);
			print("[FAT ERROR] %s NOT found.\r\n", name);
			return null;
		}
	}

	return current;
}

