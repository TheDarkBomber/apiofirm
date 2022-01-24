#include "memory.h"

void far* memcpy(void far* destination, const void far* source, uint16_t number) {
	uint8_t far* unsigned8destination = (uint8_t far*)destination;
	const uint8_t far* unsigned8source = (const uint8_t far*)source;

	for (uint16_t i = 0; i < number; i++)
		unsigned8destination[i] = unsigned8source[i];

	return destination;
}

void far* memset(void far* pointer, int value, uint16_t number) {
	uint8_t far* unsigned8pointer = (uint8_t far*)pointer;

	for (uint8_t i = 0; i < number; i++)
		unsigned8pointer[i] = (uint8_t)value;

	return pointer;
}

int memcmp(const void far *pointer1, const void far *pointer2, uint16_t number) {
	const uint8_t far* unsigned8pointer1 = (const uint8_t far*)pointer1;
	const uint8_t far *unsigned8pointer2 = (const uint8_t far *)pointer2;

	for (uint16_t i = 0; i < number; i++) {
		if (unsigned8pointer1[i] != unsigned8pointer2[i]) return 1;
	}

	return 0;
}
