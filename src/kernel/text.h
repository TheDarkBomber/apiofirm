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

typedef void (*CharacterOut)(char c);
typedef void (*StringOut)(char* s);

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
void strput(char* s);
void printf(const char* format, ...);
void printfv(const char* format, va_list arguments);
void prints(const char* format, ...);
void printsv(const char* format, va_list arguments);
void printvc(const char* format, va_list arguments, CharacterOut cout, StringOut sout);
void SetCursor(int x, int y);

extern TextContext TextCTX;

#endif
