#ifndef __APIOFIRM_COMMON_H_
#define __APIOFIRM_COMMON_H_
#include <stdint.h>

typedef struct {
	uint8_t Signature[8];
	uint8_t Checksum;
	uint8_t OEMId[6];
	uint8_t Revision;
	uint32_t RSDTAddress;
	uint32_t Length;
	uint64_t XSDTAddress;
	uint8_t FurtherChecksum;
	uint8_t RSVP[3];
} __attribute__((packed)) RSD;

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
	RSD* RSDP;
} BootInfo;

#endif
