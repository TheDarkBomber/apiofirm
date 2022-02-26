#include "gdt.h"
#include "memory.h"
#include "paging.h"
#include <stdint.h>

static GDTEntry GDT[6];
MetaGDT metaGDT;
static TSSEntry TSS;

void MakeGDTEntry(uint32_t index, uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity) {
	memset(&GDT[index], 0, sizeof(GDTEntry));
	GDT[index].LowerBase = (uint16_t)(base & 0xFFFF);
	GDT[index].MedianBase = (uint16_t)((base >> 16) & 0xFF);
	GDT[index].HigherBase = (uint16_t)((base >> 24) & 0xFF);
	GDT[index].Limit = (uint16_t)(limit & 0xFFFF);

	GDT[index].Flags = access;
	GDT[index].Granularity = ((limit >> 16) & 0x0F);
	GDT[index].Granularity |= granularity & 0xF0;
}

void InitialiseGDT() {
	metaGDT.Size = (sizeof(GDTEntry) * 6) - 1;
	metaGDT.GDT = (uint32_t)&GDT[0];

	MakeGDTEntry(0, 0, 0, 0, 0);

	MakeGDTEntry(1, 0, 0xFFFFFFFF, GDT_READ_WRITE | GDT_EXECUTABLE | GDT_CODE_DATA | GDT_MEMORY, GDT_4K | GDT_X32 | GDT_HIGH_LIMIT);
	MakeGDTEntry(2, 0, 0xFFFFFFFF, GDT_READ_WRITE | GDT_CODE_DATA | GDT_MEMORY, GDT_4K | GDT_X32 | GDT_HIGH_LIMIT);

	MakeGDTEntry(3, 0, 0xFFFFFFFF, GDT_READ_WRITE | GDT_EXECUTABLE | GDT_CODE_DATA | GDT_MEMORY | GDT_USERSPACE, GDT_4K | GDT_X32 | GDT_HIGH_LIMIT);
	MakeGDTEntry(4, 0, 0xFFFFFFFF, GDT_READ_WRITE | GDT_CODE_DATA | GDT_MEMORY | GDT_USERSPACE, GDT_4K | GDT_X32 | GDT_HIGH_LIMIT);
	WriteTSS(5);
	FlushGDT();
	FlushTSS();
}

void WriteTSS(uint32_t index) {
	uint32_t base = (uint32_t) &TSS;
	MakeGDTEntry(index, base, base + sizeof(TSSEntry), GDT_ACCESS | GDT_EXECUTABLE | GDT_USERSPACE | GDT_MEMORY, 0);

	memset(&TSS, 0, sizeof(TSSEntry));
	TSS.SS0 = 0x10;
	TSS.ESP0 = 0x00;
	TSS.CR3 = PAGE_DIRECTORY;
	TSS.ES = 0x10;
	TSS.CS = 0x08;
	TSS.SS = 0x10;
	TSS.DS = 0x10;
	TSS.FS = 0x10;
	TSS.GS = 0x10;
	TSS.IOMAP = sizeof(TSSEntry);
}

void SetKernelStack(uint32_t stack) {
	TSS.ESP0 = stack;
}
