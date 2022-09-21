#ifndef __APIOFIRM_STRING_H_
#define __APIOFIRM_STRING_H_
#include <stdint.h>
#include <stdbool.h>

char* strchr(char* str, char chr);
char* strcpy(char* destination, char* src);
uint64_t strlen(char* str);
bool streq(char* a, char* b);

bool isLower(char c);
char upcase(char c);

#endif
