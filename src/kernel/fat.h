#ifndef __APIOFIRM_FAT32_H_
#define __APIOFIRM_FAT32_H_
#include "disk.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	char JMPSHORT3CNOP[3];
	char OEMIdentifier[8];
	uint16_t BytesPerSector;
	uint8_t SectorsPerCluster;
	uint16_t ReservedSectors;
	uint8_t FATAmount;
	uint16_t RootDirectoryEntries;
	uint16_t TotalLogicalSectors;
	uint8_t MediaDescriptorType;
	uint16_t LegacySectorsPerFAT;
	uint16_t SectorsPerTrack;
	uint16_t HeadAmount;
	uint32_t HiddenSectors;
	uint32_t LargeSectorAmount;
	uint32_t SectorsPerFAT;
	uint16_t Flags;
	uint16_t Version;
	uint32_t RootCluster;
	uint16_t FSInfoSector;
	uint16_t BackupBootSector;
	char RSVP[12];
	uint8_t DriveNumber;
	uint8_t NTFlags;
	uint8_t Signature;
	uint32_t VolumeSerial;
	char VolumeLabel[11];
	char SystemIdentifier[8];
} __attribute__((packed)) FAT32Header;

typedef struct {
	uint32_t LeadSignature;
	char RSVP0[480];
	uint32_t AlternateSignature;
	uint32_t LastKnownFreeClusterAmount;
	uint32_t StartClusterHint;
	char RSVP1[12];
	uint32_t TrailSignature;
} __attribute__((packed)) FAT32FSInfo;

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
} __attribute__((packed)) FAT32DirectoryEntry;

typedef struct {
	bool Directory;
	bool Open;
	uint32_t Handle;
	uint32_t Position;
	uint32_t Size;
	uint32_t FirstCluster;
	uint32_t CurrentCluster;
	uint32_t CurrentSectorInCluster;
	char* Buffer;
} FAT32File;

typedef struct {
	FAT32Header Header;
	FAT32File Root;
	FAT32File* Files;
	Partition* Partition;
	char* FAT;
	uint32_t FileHandles;
	uint32_t FirstUsableCluster;
} FAT32Driver;

typedef enum {
	FATReadOnly = 0x01,
	FATHidden = 0x02,
	FATSystem = 0x04,
	FATVolumeID = 0x08,
	FATDirectory = 0x10,
	FATArchive = 0x20,
	FATLongName = FATReadOnly | FATHidden | FATSystem | FATVolumeID
} FATAttributes;

uint64_t FAT32ClusterToLBA(FAT32Driver* driver, uint32_t cluster);
FAT32Driver* InitialiseFAT32(Partition* partition);
uint32_t FAT32Open(FAT32Driver* driver, char* path);
uint64_t FAT32Read(FAT32Driver* driver, uint32_t handle, uint32_t amount, char* data);
bool FAT32ReadEntry(FAT32Driver* driver, uint32_t handle, FAT32DirectoryEntry* entry);
void FAT32Close(FAT32Driver* driver, uint32_t handle);

bool FAT32ReadBootSector(FAT32Driver* driver);
bool FAT32ReadTable(FAT32Driver* driver);
uint32_t FAT32OpenEntry(FAT32Driver* driver, FAT32DirectoryEntry* entry);
uint32_t FAT32NextCluster(FAT32Driver* driver, uint32_t cluster);
bool FAT32FindFile(FAT32Driver* driver, uint32_t handle, char* name, FAT32DirectoryEntry* entry);

#endif
