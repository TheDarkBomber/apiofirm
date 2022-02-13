#include "stdio.h"
#include "x86.h"

#include <stdarg.h>
#include <stdbool.h>

const unsigned SCREEN_WIDTH = 80;
const unsigned SCREEN_HEIGHT = 25;
const unsigned TAB_WIDTH = 4;
const unsigned TAB_HEIGHT = 4;

uint8_t DefaultColour = VGA_YELLOW;

uint8_t *ScreenBuffer = (uint8_t *)0xB8000;
int ScreenX = 0, ScreenY = 0;

void charset(int x, int y, char c) { ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c; } 
void colourset(int x, int y, uint8_t colour) {
	ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = colour;
}

char charget(int x, int y) { return ScreenBuffer[2 * (y * SCREEN_WIDTH + x)]; }
char colourget(int x, int y) {
	return ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1];
}

void cursorset(int x, int y) {
	int position = y * SCREEN_WIDTH + x;

	x86Output(0x3D4, 0x0F);
	x86Output(0x3D5, (uint8_t)(position & 0xFF));
	x86Output(0x3D4, 0x0e);
	x86Output(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

void clearscreen() {
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			charset(x, y, '\0');
			colourset(x, y, DefaultColour);
		}
	}

	ScreenX = 0;
	ScreenY = 0;
	cursorset(ScreenX, ScreenY);
}

void scroll(int lines) {
	for (int y = lines; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			charset(x, y - lines, charget(x, y));
			colourset(x, y - lines, colourget(x, y));
		}
	}

	for (int y = SCREEN_HEIGHT - lines; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			charset(x, y, '\0');
			colourset(x, y, DefaultColour);
		}
	}

	ScreenY -= lines;
}

void charput(char c) {
	switch (c) {
	case '\n':
		ScreenY++;
		break;

	case '\r':
		ScreenX = 0;
		break;

	case '\t':
		ScreenX += TAB_WIDTH;
		break;

	case '\v':
		ScreenY += TAB_HEIGHT;
		break;

	default:
		charset(ScreenX, ScreenY, c);
		colourset(ScreenX, ScreenY, DefaultColour);
		ScreenX++;
		break;
	}

	if (ScreenX >= SCREEN_WIDTH) {
		ScreenY++;
		ScreenX -= SCREEN_WIDTH;
	}

	if (ScreenY >= SCREEN_HEIGHT) scroll(1);
	cursorset(ScreenX, ScreenY);
}

void strput(const char* s) {
	while (*s) {
		charput(*s);
		s++;
	}
}

const char HEXADECIMAL_CHARACTERS[] = "0123456789ABCDEF";
const char NIFTIMAL_CHARACTERS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

usiprint(unsigned long long number, int radix) {
	char buffer[32];
	int position = 0;

	do {
		unsigned long long remainder = number % radix;
		number /= radix;
		buffer[position++] = NIFTIMAL_CHARACTERS[remainder];
	} while (number > 0);

	while (--position >= 0) charput(buffer[position]);
}

siprint(long long number, int radix) {
	if (number < 0) {
		charput('-');
		usiprint(-number, radix);
	} else usiprint(number, radix);
}

// Print function

#define PRINT_NORMAL_STATE 0
#define PRINT_LENGTH_STATE 1
#define PRINT_SHORT_LENGTH_STATE 2
#define PRINT_LONG_LENGTH_STATE 3
#define PRINT_SPECIFIER_STATE 4

#define PRINT_DEFAULT_LENGTH 0
#define PRINT_SSHORT_LENGTH 1
#define PRINT_SHORT_LENGTH 2
#define PRINT_LONG_LENGTH 3
#define PRINT_LLONG_LENGTH 4

void print(const char* format, ...) {
	va_list arguments;
	va_start(arguments, format);

	int state = PRINT_NORMAL_STATE;
	int length = PRINT_DEFAULT_LENGTH;
	int radix = 10;
	bool sign = false;
	bool number = false;

	while (*format) {
		switch (state) {
		case PRINT_NORMAL_STATE:
			switch (*format) {
			case '%':
				state = PRINT_LENGTH_STATE;
				break;
			default:
					charput(*format);
				break;
			}
			break;

		case PRINT_LENGTH_STATE:
			switch (*format) {
			case 'h':
				length = PRINT_SHORT_LENGTH;
				state = PRINT_SHORT_LENGTH_STATE;
				break;
			case 'l':
				length = PRINT_LONG_LENGTH_STATE;
				break;
			default:
				goto PRINT_SPECIFIER_STATE_;
			}
			break;

		case PRINT_SHORT_LENGTH_STATE:
			if (*format == 'h') {
				length = PRINT_SSHORT_LENGTH;
				state = PRINT_SPECIFIER_STATE;
			} else goto PRINT_SPECIFIER_STATE_;
			break;

		case PRINT_LONG_LENGTH_STATE:
			if (*format == 'l') {
				length = PRINT_LLONG_LENGTH;
				state = PRINT_SPECIFIER_STATE;
			} else goto PRINT_SPECIFIER_STATE_;
			break;

		case PRINT_SPECIFIER_STATE:
		PRINT_SPECIFIER_STATE_:
			switch(*format) {
			case 'c':
				charput((char)va_arg(arguments, int));
				break;
			case 's':
				strput(va_arg(arguments, const char*));
				break;
			case '%':
				charput('%');
				break;
			case 'd':
			case 'i':
				radix = 10;
				sign = true;
				number = true;
				break;
			case 'u':
				radix = 10;
				sign = false;
				number = true;
				break;
			case 'X':
			case 'x':
				radix = 16;
				sign = false;
				number = true;
				break;
			case 'o':
				radix = 8;
				sign = false;
				number = true;
				break;
			case 'b':
				radix = 2;
				sign = false;
				number = true;
				break;
			case 'a':
				radix = 6;
				sign = false;
				number = true;
				break;
			case 'A':
				radix = 36;
				sign = false;
				number = true;
				break;
			default: break;
			}

			if (number) {
				if (sign) {
					switch (length) {
					case PRINT_SSHORT_LENGTH:
					case PRINT_SHORT_LENGTH:
					case PRINT_DEFAULT_LENGTH:
						siprint(va_arg(arguments, int), radix);
						break;

					case PRINT_LONG_LENGTH:
						siprint(va_arg(arguments, long), radix);
						break;

					case PRINT_LLONG_LENGTH:
						siprint(va_arg(arguments, long long), radix);
						break;
					}
				} else {
					switch (length) {
					case PRINT_SSHORT_LENGTH:
					case PRINT_SHORT_LENGTH:
					case PRINT_DEFAULT_LENGTH:
						usiprint(va_arg(arguments, unsigned int), radix);
						break;

					case PRINT_LONG_LENGTH:
						usiprint(va_arg(arguments, unsigned long), radix);
						break;

					case PRINT_LLONG_LENGTH:
						usiprint(va_arg(arguments, unsigned long long), radix);
						break;
					}
				}
			}

			state = PRINT_NORMAL_STATE;
			length = PRINT_DEFAULT_LENGTH;
			radix = 10;
			sign = false;
			break;
		}

		format++;
	}

	va_end(arguments);
}

void bufferprint(const char *message, const void *buffer, uint16_t count) {
	const uint8_t* unsigned8buffer = (const uint8_t *)buffer;
	strput(message);
	for (uint16_t i = 0; i < count; i++) {
		charput(HEXADECIMAL_CHARACTERS[unsigned8buffer[i] >> 4]);
		charput(HEXADECIMAL_CHARACTERS[unsigned8buffer[i] & 0xF]);
	}
	strput("\r\n");
}

void setprintPosition(int x, int y) {
	ScreenX = x;
	ScreenY = y;
	cursorset(x, y);
}

void setDefaultColour(uint8_t colour) {
	DefaultColour = colour;
}
