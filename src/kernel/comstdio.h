#ifndef __SERIAL_STDIO_H
#define __SERIAL_STDIO_H

#include <stdint.h>

void comstrput(const char *s);
void cbufferprint(const char *message, const void *buffer, uint32_t count);
void cprint(const char *format, ...);
void comget(char* buffer, char stop);

#endif
