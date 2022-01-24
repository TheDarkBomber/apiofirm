#ifndef __STDIO_STD_H
#define __STDIO_STD_H
#include "stdint.h"
void charput(char c);
void strput(const char *s);
void farstrput(const char far *s);
void cdecl print(const char* format, ...);
void bufferprint(const char* message, const void far* buffer, uint16_t count);
#endif
