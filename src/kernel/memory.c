#include "memory.h"

char* memcpy(char* destination, char* src, uint64_t size) {
	for (uint64_t i = 0; i < size; i++) destination[i] = src[i];
	return destination;
}

char* memset(char* pointer, char value, uint64_t size) {
	for (uint64_t i = 0; i < size; i++) pointer[i] = value;
	return pointer;
}

int memcmp(char* p1, char* p2, uint64_t size) {
	for (uint64_t i = 0; i < size; i++) {
		if (p1[i] != p2[i]) return 1;
	}

	return 0;
}
