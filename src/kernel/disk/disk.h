#ifndef __APIOFIRM_DISK_H_
#define __APIOFIRM_DISK_H_
#include <disk/gpt.h>
#include <stdbool.h>

typedef bool (*DiskAPI_ReadSector)(void* driver, uint64_t LBA, char* data);
typedef bool (*DiskAPI_WriteSector)(void* driver, uint64_t LBA, char* data);
typedef bool (*DiskAPI_ReadSectors)(void* driver, uint64_t LBA, uint64_t amount, char* data);
typedef bool (*DiskAPI_WriteSectors)(void* driver, uint64_t LBA, uint64_t amount, char* data);


struct Disk;

typedef struct {
	struct Disk* Disk;
	GPTEntry Header;
} Partition;

typedef struct {
	DiskAPI_ReadSector ReadSingle;
	DiskAPI_WriteSector WriteSingle;
	DiskAPI_ReadSectors Read;
	DiskAPI_WriteSectors Write;
} DiskAPI;

typedef struct Disk {
	GPTHeader Table;
	DiskAPI* API;
	void* Driver;
	Partition** Partitions;
	uint64_t ID;
	uint64_t PartitionAmount;
} Disk;

extern Disk* DiskRegistry;
extern uint64_t DiskRegistryLength;

bool ReadDiskSectors(Disk* disk, uint64_t LBA, uint64_t amount, char* data);
bool WriteDiskSectors(Disk* disk, uint64_t LBA, uint64_t amount, char* data);

void InitialiseDisk(Disk* disk);

bool ReadPartitionSectors(Partition* partition, uint64_t LBA, uint64_t amount, char* data);
bool WritePartitionSectors(Partition* partition, uint64_t LBA, uint64_t amount, char* data);


#endif
