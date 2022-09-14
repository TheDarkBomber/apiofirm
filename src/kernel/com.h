#ifndef __APIOFIRM_COMMON_H_
#define __APIOFIRM_COMMON_H_
#include <stdint.h>

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
} BootInfo;

#endif
