#include "memory.h"

void* memcpy(void* destination, const void* source, uint32_t number) {
	uint8_t * unsigned8destination = (uint8_t *)destination;
	const uint8_t * unsigned8source = (const uint8_t *)source;

	for (uint32_t i = 0; i < number; i++)
		unsigned8destination[i] = unsigned8source[i];

	return destination;
}

void* memset(void* pointer, int value, uint16_t number) {
	uint8_t* unsigned8pointer = (uint8_t *)pointer;

	for (uint8_t i = 0; i < number; i++)
		unsigned8pointer[i] = (uint8_t)value;

	return pointer;
}

int memcmp(const void* pointer1, const void* pointer2, uint16_t number) {
	const uint8_t* unsigned8pointer1 = (const uint8_t *)pointer1;
	const uint8_t* unsigned8pointer2 = (const uint8_t *)pointer2;

	for (uint16_t i = 0; i < number; i++) {
		if (unsigned8pointer1[i] != unsigned8pointer2[i]) return 1;
	}

	return 0;
}
