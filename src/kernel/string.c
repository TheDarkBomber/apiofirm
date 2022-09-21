#include "string.h"
#include <stddef.h>

char* strchr(char* str, char chr) {
	if (str == NULL) return NULL;

	while (*str) {
		if (*str == chr) return str;
		++str;
	}

	return NULL;
}

char* strcpy(char* destination, char* src) {
	char* ogdestination = destination;
	if (destination == NULL) return NULL;
	if (src == NULL) {
		*destination = '\0';
		return destination;
	}

	while (*src) {
		*destination = *src;
		++src, ++destination;
	}

	*destination = '\0';
	return ogdestination;
}

uint64_t strlen(char* str) {
	uint64_t length = 0;
	while (*str) {
		++length, ++str;
	}
	return length;
}

bool streq(char* a, char* b) {
	if (strlen(a) != strlen(b)) return false;
	while (*a) {
		if (*a != *b) return false;
		++a, ++b;
	}
	return true;
}

bool isLower(char c) { return c >= 'a' && c <= 'z'; }
char upcase(char c) { return isLower(c) ? (c - 32) : c; }

