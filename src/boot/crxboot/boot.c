#include "uefi/uefi.h"
#include <stddef.h>

#define ELF_MAGIC_NUMBER "\177ELF"
#define ELF_MAGIC_LENGTH 4
#define EI_CLASS 4
#define ELFCLASS64 2
#define EI_DATA 5
#define ELFDATA2LSB 1
#define ET_EXEC 2
#define PT_LOAD 1
#define EM_MACH 62

typedef struct {
	uint8_t Identifier[16];
	uint16_t Type;
	uint16_t Machine;
	uint32_t Version;
	uint64_t Entrypoint;
	uint64_t ProgramHeaderOffset;
	uint64_t SectionHeaderOffset;
	uint32_t Flags;
	uint16_t ElfHeaderSize;
	uint16_t ProgramHeaderSize;
	uint16_t ProgramHeaderCount;
	uint16_t SectionHeaderSize;
	uint16_t SectionHeaderCount;
	uint16_t SectionStringIndex;
} Elf64Ehdr;

typedef struct {
	uint32_t Type;
	uint32_t Flags;
	uint64_t Offset;
	uint64_t VirtualAddress;
	uint64_t PhysicalAddress;
	uint64_t FileSize;
	uint64_t MemorySize;
	uint64_t Alignment;
} Elf64Phdr;

typedef struct {
	unsigned* FrameBuffer;
	unsigned Width;
	unsigned Height;
	unsigned Pitch;
} VideoOut;

typedef struct {
	char Identifier[2];
	char Mode;
	char Height;
	char* Font;
} FontPSF1;

typedef struct {
	int argc;
	char** argv;
	FontPSF1 TFX;
	VideoOut GFX;
	void* MMap;
	uint64_t mmapSize;
	uint64_t mmapDSize;
	char* RSDP;
} BootInfo;

static void plotPixel(VideoOut* gfx, int x, int y, unsigned long pixel) {
  *((uint32_t *)(gfx->FrameBuffer + 4 * gfx->Pitch * y + 4 * x)) = pixel;
}

int main(int argc, char** argv) {
	FILE* kernel;
	unsigned long ksize;
	char* kbuffer;

	printf("[CRXBOOT] Initialised.\n");

	if ((kernel = fopen("\\apiofirm\\bin\\system.k", "r"))) {
		fseek(kernel, 0, SEEK_END);
		ksize = ftell(kernel);
		fseek(kernel, 0, SEEK_SET);
		printf("[CRXBOOT] Read kernel, size is 0x%x.\n", (unsigned)ksize);
		kbuffer = malloc(ksize + 1);
		if (!kbuffer) {
			printf("[CRXBOOT] Unable to allocate memory.\n");
			fclose(kernel);
			return 1;
		}
		fread(kbuffer, ksize, 1, kernel);
		fclose(kernel);
	} else {
		printf("[CRXBOOT] Unable to open handle to kernel.\n");
		return 0;
	}

	BootInfo boot;
	efi_guid_t GOPGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	efi_gop_t* GOP = NULL;
	memset(&boot, 0, sizeof(BootInfo));
	efi_status_t status = BS->LocateProtocol(&GOPGUID, NULL, (void**)&GOP);
	if (!EFI_ERROR(status) && GOP) {
		status = GOP->SetMode(GOP, 0);
		if (EFI_ERROR(status)) {
			printf("[CRXBOOT] Unable to set video mode.\n");
			return 0;
		}

		boot.GFX.FrameBuffer = (unsigned*)GOP->Mode->FrameBufferBase;
		boot.GFX.Width = GOP->Mode->Information->HorizontalResolution;
		boot.GFX.Height = GOP->Mode->Information->VerticalResolution;
		boot.GFX.Pitch = GOP->Mode->Information->PixelsPerScanLine;
	} else {
		printf("[CRXBOOT] Unable to get graphics output protocol.");
		return 0;
	}

	if (argc > 1) {
		boot.argc = argc - 1;
		boot.argv = (char**)malloc(argc * sizeof(char*));

		if (boot.argv) {
			for (unsigned i = 0; i < boot.argc; i++) {
				if ((boot.argv[i] = (char*)malloc(strlen(argv[i + 1]) + 1)))
					strcpy(boot.argv[i], argv[i + 1]);
			}
		}
	}

	Elf64Ehdr* ELF = (Elf64Ehdr*)kbuffer;
	Elf64Phdr* PHDR;
	uintptr_t ElfEntryPoint;

	if (!memcmp(ELF->Identifier, ELF_MAGIC_NUMBER, ELF_MAGIC_LENGTH)
		&& ELF->Identifier[EI_CLASS] == ELFCLASS64
		&& ELF->Identifier[EI_DATA] == ELFDATA2LSB
		&& ELF->Type == ET_EXEC
		&& ELF->Machine == EM_MACH
		&& ELF->ProgramHeaderCount > 0) {
		unsigned i;
		for (PHDR = (Elf64Phdr*)(kbuffer + ELF->ProgramHeaderOffset), i = 0; i < ELF->ProgramHeaderCount; i++, PHDR = (Elf64Phdr*)((uint8_t*)PHDR + ELF->ProgramHeaderSize)) {
			if (PHDR->Type == PT_LOAD) {
				memcpy((void*)PHDR->VirtualAddress, kbuffer + PHDR->Offset, PHDR->FileSize);
				memset((void*)(PHDR->PhysicalAddress + PHDR->FileSize), 0, PHDR->MemorySize - PHDR->FileSize);
			}
		}
		ElfEntryPoint = ELF->Entrypoint;
	} else {
		printf("[CRXBOOT] EI_CLASS %x\n", ELF->Identifier[EI_CLASS]);
		printf("[CRXBOOT] EI_DATA %x\n", ELF->Identifier[EI_DATA]);
		printf("[CRXBOOT] TYPE %x\n", ELF->Type);
		printf("[CRXBOOT] MACHINE %x\n", ELF->Machine);
		printf("[CRXBOOT] PHC %x\n", ELF->ProgramHeaderCount);
		printf("[CRXBOOT] Enter at 0x%x\n", (unsigned int)ElfEntryPoint);
		printf("[CRXBOOT] Not a bootable ELF AMD64 executable.\n");
		return 0;
	}

	free(kbuffer);

	printf("[CRXBOOT] Loaded kernel.\n");

	printf("[CRXBOOT] Loading font data.\n");
	FILE* consoleFont;
	if ((consoleFont = fopen("\\apiofirm\\res\\confon.psf", "r"))) {
		fread(&boot.TFX, 1, offsetof(FontPSF1, Font), consoleFont);
		boot.TFX.Font = malloc(boot.TFX.Height * 256);
		memcpy(boot.TFX.Font, &boot.TFX, 4);
		fread(boot.TFX.Font + 4, 1, boot.TFX.Height * 256, consoleFont);
		fclose(consoleFont);
	} else {
		printf("[CRXBOOT] Failed to open font!\n");
		return 0;
	}

	printf("[CRXBOOT] Fetching memory map.\n");

	efi_memory_descriptor_t* MMap = NULL;
	uintn_t mmapSize = 0, mapKey = 0, mmapDSize = 0;

	status = BS->GetMemoryMap(&mmapSize, NULL, &mapKey, &mmapDSize, NULL);
	if (status != EFI_BUFFER_TOO_SMALL || !mmapSize) {
		printf("[CRXBOOT] Failed to load memory map.\n");
		return 1;
	}

	mmapSize += 4 * mmapDSize;
	MMap = (efi_memory_descriptor_t*)malloc(mmapSize);
	if (!MMap) {
		printf("[CRXBOOT] Failed to allocate memory for the map of the memory.\n");
		return 1;
	}

	status = BS->GetMemoryMap(&mmapSize, MMap, &mapKey, &mmapDSize, NULL);
	if (EFI_ERROR(status)) {
		printf("[CRXBOOT] Failed to correctly load memory map.\n");
		return 2;
	}

	boot.MMap = MMap;
	boot.mmapSize = mmapSize;
	boot.mmapDSize = mmapDSize;

	efi_configuration_table_t* cfgTable = ST->ConfigurationTable;
	char* RSDP = NULL;
	efi_guid_t ACPITableGUID = ACPI_20_TABLE_GUID;

	for (uintn_t i = 0; i < ST->NumberOfTableEntries; i++) {
		if (memcmp((void*)&cfgTable[i].VendorGuid, (void*)&ACPITableGUID, sizeof(efi_guid_t))) {
			if (!memcmp("RSD PTR ", cfgTable->VendorTable, 8)) RSDP = cfgTable->VendorTable;
		}
		cfgTable++;
	}

	boot.RSDP = RSDP;

	printf("[CRXBOOT] Executing kernel.\n");
	((void(* __attribute__((sysv_abi)))(BootInfo*))(ElfEntryPoint))(&boot);
	printf("[CRXBOOT] Finished kernel.\n");

	for(;;);
	return 6;
}
