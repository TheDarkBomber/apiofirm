#include "fat.h"
#include "memory.h"
#include "text.h"
#include "string.h"
#include <stddef.h>

uint64_t FAT32ClusterToLBA(FAT32Driver* driver, uint32_t cluster) {
	return driver->FirstUsableCluster + cluster * driver->Header.SectorsPerCluster - (2 * driver->Header.SectorsPerCluster);
}

bool FAT32ReadBootSector(FAT32Driver* driver) {
	char buffer[512];
	bool success = ReadPartitionSectors(driver->Partition, 0, 1, buffer);
	if (!success) return false;
	memcpy((char*)&driver->Header, buffer, sizeof(FAT32Header));
	return true;
}

bool FAT32ReadTable(FAT32Driver* driver) {
	return ReadPartitionSectors(driver->Partition, driver->Header.ReservedSectors, driver->Header.SectorsPerFAT, driver->FAT);
}

FAT32Driver* InitialiseFAT32(Partition* partition) {
	FAT32Driver* driver = (FAT32Driver*)mallocate(sizeof(FAT32Driver));
	driver->Partition = partition;
	driver->FileHandles = 0;
	driver->Files = NULL;
	if (!FAT32ReadBootSector(driver)) {
		printf("[FAT32] Failed to read boot sector.\n");
		mfree((char*)driver);
		return NULL;
	}
	driver->FAT = mallocate(driver->Header.BytesPerSector * driver->Header.SectorsPerFAT);
	if (!FAT32ReadTable(driver)) {
		printf("[FAT32] Failed to read file allocation table.\n");
		mfree((char*)driver);
		return NULL;
	}

	driver->FirstUsableCluster = driver->Header.ReservedSectors + (driver->Header.FATAmount * driver->Header.SectorsPerFAT);
	uint64_t rootLBA = FAT32ClusterToLBA(driver, driver->Header.RootCluster);
	uint64_t rootSize = sizeof(FAT32DirectoryEntry) * driver->Header.RootDirectoryEntries;
	driver->Root.Handle = ~0;
	driver->Root.Directory = true;
	driver->Root.Position = 0;
	driver->Root.Size = rootSize;
	driver->Root.Open = true;
	driver->Root.FirstCluster = rootLBA;
	driver->Root.CurrentCluster = rootLBA;
	driver->Root.CurrentSectorInCluster = 0;
	driver->Root.Buffer = mallocate(512);

	if (!ReadPartitionSectors(driver->Partition, rootLBA, 1, driver->Root.Buffer)) {
		printf("[FAT32] Failed to read root directory.\n");
		mfree(driver->Root.Buffer);
		mfree((char*)driver);
		return NULL;
	}

	return driver;
}

void FAT32Close(FAT32Driver* driver, uint32_t handle) {
	if (handle == ~0) {
		driver->Root.Position = 0;
		driver->Root.CurrentCluster = driver->Root.FirstCluster;
	} else {
		driver->Files[handle].Open = false;
		mfree(driver->Files[handle].Buffer);
		driver->FileHandles--;
		driver->Files = (FAT32File*)mreallocate((char*)driver->Files, driver->FileHandles * sizeof(FAT32File));
	}
}

uint32_t FAT32OpenEntry(FAT32Driver* driver, FAT32DirectoryEntry* entry) {
	uint32_t handle = driver->FileHandles++;
	driver->Files = (FAT32File*)mreallocate((char*)driver->Files, driver->FileHandles * sizeof(FAT32File));
	FAT32File* file = &driver->Files[handle];
	file->Handle = handle;
	file->Directory = (entry->Attributes & FATDirectory);
	file->Position = 0;
	file->Size = entry->Size;
	file->FirstCluster = entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
	file->CurrentCluster = file->FirstCluster;
	file->CurrentSectorInCluster = 0;
	file->Buffer = mallocate(512);

	if (!ReadPartitionSectors(driver->Partition, FAT32ClusterToLBA(driver, file->CurrentCluster), 1, file->Buffer)) {
			printf("[FAT32] Failed to open entry, cluster=%u, LBA=%u\n", file->CurrentCluster, FAT32ClusterToLBA(driver, file->CurrentCluster));
			for (int i = 0; i < 11; i++) charput(entry->Name[i]);
			FAT32Close(driver, handle);
			return ~0;
		}

	file->Open = true;
	return handle;
}

uint32_t FAT32NextCluster(FAT32Driver* driver, uint32_t cluster) {
	uint32_t index = cluster * 4;
	return (*(uint32_t*)(driver->FAT + index)) & 0x0FFFFFFF;
}

uint64_t FAT32Read(FAT32Driver* driver, uint32_t handle, uint32_t amount, char* data) {
	FAT32File* file = (handle == ~0) ? &driver->Root : &driver->Files[handle];
	if (!file->Directory || (file->Directory && file->Size != 0))
		amount = (amount < file->Size - file->Position) ? amount : file->Size - file->Position;
	char* ogdata = data;

	while (amount > 0) {
		uint32_t left = 512 - (file->Position % 512);
		uint32_t take = (amount < left) ? amount : left;
		memcpy(data, file->Buffer + file->Position % 512, take);
		data += take;
		file->Position += take;
		amount -= take;

		if (left == take) {
			if (file->Handle == ~0) {
				++file->CurrentCluster;
				if (!ReadPartitionSectors(driver->Partition, file->CurrentCluster, 1, file->Buffer)) {
					printf("[FAT32] Read failure.\n");
					break;
				}
			} else {
				if (++file->CurrentSectorInCluster >= driver->Header.SectorsPerCluster) {
					file->CurrentSectorInCluster = 0;
					file->CurrentCluster = FAT32NextCluster(driver, file->CurrentCluster);
				}

				if (file->CurrentCluster >= 0x0FFFFFF8) {
					file->Size = file->Position;
					break;
				}

				if (!ReadPartitionSectors(driver->Partition, FAT32ClusterToLBA(driver, file->CurrentCluster) + file->CurrentSectorInCluster, 1, file->Buffer)) {
					printf("[FAT32] Read failure (2).\n");
					break;
				}
			}
		}
	}

	return data - ogdata;
}

bool FAT32ReadEntry(FAT32Driver* driver, uint32_t handle, FAT32DirectoryEntry* entry) {
	return FAT32Read(driver, handle, sizeof(FAT32DirectoryEntry), (char*)entry) == sizeof(FAT32DirectoryEntry);
}

bool FAT32FindFile(FAT32Driver* driver, uint32_t handle, char* name, FAT32DirectoryEntry* entry) {
	char FATName[12];
	FAT32DirectoryEntry ent;
	memset(FATName, ' ', 12);
	FATName[11] = '\0';
	char* extension = strchr(name, '.');
	if (!extension) extension = name + 11;
	for (int i = 0; i < 8 && name[i] && name + i < extension; i++)
		FATName[i] = upcase(name[i]);

	if (extension != name + 11) {
		for (int i = 0; i < 3 && extension[i + 1]; i++)
			FATName[i + 8] = upcase(extension[i + 1]);
	}

	while (FAT32ReadEntry(driver, handle, &ent)) {
		if (!memcmp(FATName, (char*)ent.Name, 11)) {
			*entry = ent;
			return true;
		}
	}

	return false;
}

uint32_t FAT32Open(FAT32Driver* driver, char* path) {
	char name[256];
	memset(name, 0, 256);

	if (path[0] == '/') path++;
	uint32_t currentHandle = ~0;

	while (*path) {
		bool last = false;
		char* delimiter = strchr(path, '/');
		if (delimiter) {
			memcpy(name, path, delimiter - path);
			name[delimiter - path + 1] = '\0';
			path = delimiter + 1;
		} else {
			uint64_t length = strlen(path);
			memcpy(name, path, length);
			name[length + 1] = '\0';
			path += length;
			last = true;
		}

		FAT32DirectoryEntry entry;
		if (FAT32FindFile(driver, currentHandle, name, &entry)) {
			FAT32Close(driver, currentHandle);
			if (!last && !(entry.Attributes & FATDirectory)) {
				printf("[FAT32] %s is not a directory.", name);
				return ~0;
			}

			currentHandle = FAT32OpenEntry(driver, &entry);
		} else {
			FAT32Close(driver, currentHandle);
			printf("[FAT32] %s not found.\n", name);
			return ~0;
		}
	}

	return currentHandle;
}
