#ifndef __APIOFIRM_x86_H_
#define __APIOFIRM_x86_H_
#include <stdint.h>

inline void x86Output(uint16_t port, uint8_t value) {
	asm ("outb %0, %1" : : "a"(value), "Nd"(port));
}

inline uint8_t x86Input(uint16_t port) {
	uint8_t returnValue;
	asm ("inb %1, %0" : "=a"(returnValue) : "Nd"(port));
	return returnValue;
}

#endif
