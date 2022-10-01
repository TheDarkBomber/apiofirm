#ifndef __APIOFIRM_GDT_H_
#define __APIOFIRM_GDT_H_
#include <stdint.h>

typedef struct {
	uint16_t Size;
	uint64_t Offset;
} __attribute__((packed)) MetaGDT;

typedef struct {
	uint16_t LowerLimit;
	uint16_t LowerBase;
	uint8_t IntermediaryBase;
	uint8_t Access;
	uint8_t HigherLimit;
	uint8_t HigherBase;
} __attribute__((packed)) GDTEntry;

typedef struct {
	GDTEntry NullSegment; // 0x00
	GDTEntry CodeSegment; // 0x08
	GDTEntry DataSegment; // 0x10
	GDTEntry UserCodeSegment; // 0x18
	GDTEntry UserDataSegment; // 0x20
	GDTEntry TaskStateSegment; // 0x28
} __attribute__((packed)) __attribute__((aligned(0x1000))) GDTStructure;

extern GDTStructure GDT;

void LoadGDT(MetaGDT* metaGDT);

#endif
