#ifndef __GDT_KERNEL_H
#define __GDT_KERNEL_H
#include <stdint.h>

#define GDT_CODE_SEGMENT 0x08
#define GDT_DATA_SEGMENT 0x10

typedef enum {
	GDT_ACCESS = 0b1,
	GDT_READ_WRITE = 0b10,
	GDT_EXPANSION = 0b100,
	GDT_EXECUTABLE = 0b1000,
	GDT_CODE_DATA = 0b10000,
	GDT_USERSPACE = 0b1100000,
	GDT_MEMORY = 0b10000000
} GDTAccess;

typedef enum {
	GDT_HIGH_LIMIT = 0x0F,
	GDT_OS = 0x10,
	GDT_X32 = 0x40,
	GDT_4K = 0x80
} GDTFlags;

#define bits(x, y) unsigned int x : y
typedef struct {
	bits(Limit, 16);
	bits(LowerBase, 16);
	bits(MedianBase, 8);
	bits(Flags, 8);
	bits(Granularity, 8);
	bits(HigherBase, 8);
} __attribute__((packed)) GDTEntry;
#undef bits

typedef struct {
	uint16_t Size;
	uint32_t GDT;
} __attribute__((packed)) MetaGDT;

typedef struct {
	uint32_t PreviousTSS;
	uint32_t ESP0;
	uint32_t SS0;
	uint32_t ESP1;
	uint32_t SS1;
	uint32_t ESP2;
	uint32_t SS2;
	uint32_t CR3;
	uint32_t EIP;
	uint32_t EFLAGS;
	uint32_t EAX;
	uint32_t ECX;
	uint32_t EDX;
	uint32_t EBX;
	uint32_t ESP;
	uint32_t EBP;
	uint32_t ESI;
	uint32_t EDI;
	uint32_t ES;
	uint32_t CS;
	uint32_t SS;
	uint32_t DS;
	uint32_t FS;
	uint32_t GS;
	uint32_t LDT;
	uint16_t TP;
	uint16_t IOMAP;
} __attribute__((packed)) TSSEntry;

void MakeGDTEntry(uint32_t index, uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity);
void WriteTSS(uint32_t index);
void SetKernelStack(uint32_t stack);
void InitialiseGDT();

extern void FlushTSS();
extern void FlushGDT();

#endif
