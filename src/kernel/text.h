#ifndef __APIOFIRM_TEXT_H_
#define __APIOFIRM_TEXT_H_
#include "com.h"
#include <stdarg.h>

typedef struct {
	VideoOut* GFX;
	FontPSF1* TFX;
	uint32_t CursorX;
	uint32_t CursorY;
	uint32_t Foreground;
	uint32_t Background;
} TextContext;

inline void plotPixel(VideoOut *gfx, int x, int y, unsigned long pixel) {
  *((uint32_t *)(gfx->FrameBuffer + (gfx->Pitch * y) + x)) = pixel;
}

inline void plotBlock(VideoOut *gfx, int x, int y, uint32_t blockSize, uint32_t colour) {
  for (int i = 0; i < blockSize; i++) {
    for (int j = 0; j < blockSize; j++) {
      plotPixel(gfx, x + i, y + j, colour);
    }
  }
}

void ClearScreen();
void charput(char c);
void strput(const char* s);
void printf(const char *fmt, ...);
void printfv(const char* fmt, va_list arguments);
void SetCursor(int x, int y);

extern TextContext TextCTX;

#endif
