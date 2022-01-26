#include "string.h"
#include <stdint.h>
#include <stddef.h>

const char* strchr(const char* str, char chr) {
	if (str == null) return null;

	while (*str) {
		if (*str == chr) return str;
		++str;
	}

	return null;
}

char* strcpy(char* destination, const char* source) {
	char* originalDestination = destination;
	if (destination == null) return null;
	if (source == null) {
		*destination = '\0';
		return destination;
	}

	while (*source) {
		*destination = *source;
		++source;
		++destination;
	}

	*destination = '\0';
	return originalDestination;
}

unsigned strlen(const char* str) {
	unsigned length = 0;
	while (*str) {
		++length;
		++str;
	}

	return length;
}
