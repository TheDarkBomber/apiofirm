#ifndef __APIOFIRM_GPT_H_
#define __APIOFIRM_GPT_H_
#include <stdint.h>

#define GPT_PROTECTIVE_MBR 0
#define GPT_HEADER 1
#define GPT_START_OF_ENTRIES 2

typedef struct {
	char Signature[8];
	uint32_t Revision;
	uint32_t HeaderSize;
	uint32_t Checksum;
	uint32_t RSVP;
	uint64_t SelfLBA;
	uint64_t AlternateLBA;
	uint64_t FirstUsableBlock;
	uint64_t LastUsableBlock;
	char GUID[16];
	uint64_t StartLBA;
	uint32_t EntryAmount;
	uint32_t EntrySize;
	uint32_t EntryChecksum;
} __attribute__((packed)) GPTHeader;

typedef struct {
	char TypeGUID[16];
	char GUID[16];
	uint64_t StartLBA;
	uint64_t EndLBA;
	uint64_t Attributes;
	uint16_t UTF16LEName[36];
} __attribute__((packed)) GPTEntry;

#endif
