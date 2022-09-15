#ifndef __APIOFIRM_PANIC_H_
#define __APIOFIRM_PANIC_H_
#include <stdint.h>

void GenericKernelPanic(const char* format, char* error, ...);

#endif
