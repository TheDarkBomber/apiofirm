#ifndef __STRING_STD_H
#define __STRING_STD_H
#include <stdint.h>

const char *strchr(const char *str, char chr);
char *strcpy(char *destination, const char *source);
unsigned strlen(const char* str);
int32_t strcmp(const char* a, const char* b);

#endif
