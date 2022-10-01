#include <disk/disk.h>
#include <memory.h>
#include <panic.h>

Disk* DiskRegistry;
uint64_t DiskRegistryLength;

bool ReadDiskSectors(Disk* disk, uint64_t LBA, uint64_t amount, char* data) {
	if (amount == 1) return disk->API->ReadSingle(disk->Driver, LBA, data);
	return disk->API->Read(disk->Driver, LBA, amount, data);
}

bool WriteDiskSectors(Disk* disk, uint64_t LBA, uint64_t amount, char* data) {
	if (amount == 1) return disk->API->WriteSingle(disk->Driver, LBA, data);
	return disk->API->Write(disk->Driver, LBA, amount, data);
}

void InitialiseDisk(Disk* disk) {
	char* buffer = mallocate(512 * 33);
	if (!ReadDiskSectors(disk, GPT_HEADER, 33, buffer)) {
		GenericKernelPanic("Failed to read GPT header from disk %u\n!", "Disk Initialisation Error", disk->ID);
	}
	memcpy((char*)&disk->Table, buffer, sizeof(GPTHeader));
	Partition partStore[32];
	char nullGUID[16] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	for (int i = 0; i < 32; i++, disk->PartitionAmount++) {
		partStore[i].Header = *(GPTEntry *)(buffer + 512 + (512 * i));
		if (!memcmp(partStore[i].Header.GUID, nullGUID, 16)) break;
		partStore[i].Disk = disk;
	}

	disk->Partitions = (Partition**)mallocate(sizeof(Partition*) * disk->PartitionAmount);
	for (int i = 0; i < disk->PartitionAmount; i++) {
		disk->Partitions[i] = (Partition*)mallocate(sizeof(Partition));
		memcpy((char*)disk->Partitions[i], (char*)&partStore[i], sizeof(Partition));
	}
	mfree(buffer);
}

bool ReadPartitionSectors(Partition* partition, uint64_t LBA, uint64_t amount, char* data) {
	return ReadDiskSectors(partition->Disk, partition->Header.StartLBA + LBA, amount, data);
}

bool WritePartitionSectors(Partition* partition, uint64_t LBA, uint64_t amount, char* data) {
	return WriteDiskSectors(partition->Disk, partition->Header.StartLBA + LBA, amount, data);
}
