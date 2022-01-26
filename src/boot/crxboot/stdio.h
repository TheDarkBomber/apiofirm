#ifndef __STDIO_STD_H
#define __STDIO_STD_H
#include <stdint.h>
void clearscreen();
void charput(char c);
void strput(const char *s);
void printf(const char *format, ...);
void bufferprint(const char* message, const void* buffer, uint16_t count);
#endif
