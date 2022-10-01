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

inline void x86OutputWide(uint16_t port, uint16_t value) {
	asm ("outw %0, %1" : : "a"(value), "Nd"(port));
}

inline uint16_t x86InputWide(uint16_t port) {
	uint16_t returnValue;
	asm ("inw %1, %0" : "=a"(returnValue) : "Nd"(port));
	return returnValue;
}

inline void x86OutputDouble(uint16_t port, uint32_t value) {
	asm ("outl %0, %1" : : "a"(value), "Nd"(port));
}

inline uint32_t x86InputDouble(uint16_t port) {
	uint32_t returnValue;
	asm ("inl %1, %0" : "=a"(returnValue) : "Nd"(port));
	return returnValue;
}

#endif
