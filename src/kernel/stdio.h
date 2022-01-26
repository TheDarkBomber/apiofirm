#ifndef __STDIO_STD_H
#define __STDIO_STD_H
#include <stdint.h>

#define VGA_BLACK 0
#define VGA_BLUE 1
#define VGA_GREEN 2
#define VGA_CYAN 3
#define VGA_RED 4
#define VGA_MAGENTA 5
#define VGA_BROWN 6
#define VGA_LIGHT_GRAY 7
#define VGA_DARK_GRAY 8
#define VGA_LIGHT_BLUE 9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN 11
#define VGA_LIGHT_RED 12
#define VGA_LIGHT_MAGENTA 13
#define VGA_YELLOW 14
#define VGA_WHITE 15

void clearscreen();
void charput(char c);
void strput(const char *s);
void printf(const char *format, ...);
void bufferprint(const char* message, const void* buffer, uint16_t count);
void setprintPosition(int x, int y);
void setDefaultColour(uint8_t colour);
#endif
