#include "disk.h"

Disk* DiskRegistry;
uint64_t DiskRegistryLength;

bool ReadDiskSectors(Disk* disk, uint64_t LBA, uint64_t amount, char* data) {
	if (amount == 1) return disk->Driver->ReadSingle(disk->Driver->Controller, LBA, data);
	return disk->Driver->Read(disk->Driver->Controller, LBA, amount, data);
}

bool WriteDiskSectors(Disk* disk, uint64_t LBA, uint64_t amount, char* data) {
	if (amount == 1) return disk->Driver->WriteSingle(disk->Driver->Controller, LBA, data);
	return disk->Driver->Write(disk->Driver->Controller, LBA, amount, data);
}
