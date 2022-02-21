#include "ctype.h"

bool isLower(char c) { return c >= 'a' && c <= 'z'; }
char upcase(char c) { return isLower(c) ? (c - 'a' + 'A') : c; }
