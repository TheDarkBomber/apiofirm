#include "serial.h"
#include "comstdio.h"
#include <stdarg.h>
#include <stdint.h>

void comstrput(const char* s) {
	while (*s) {
		SerialWrite(*s);
		s++;
	}
}

const char CHEXADECIMAL_CHARACTERS[] = "0123456789ABCDEF";
const char CNIFTIMAL_CHARACTERS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void comusiprint(unsigned long long number, int radix) {
	char buffer[32];
	int position = 0;

	do {
		unsigned long long remainder = number % radix;
		number /= radix;
		buffer[position++] = CNIFTIMAL_CHARACTERS[remainder];
	} while (number > 0);

	while (--position >= 0) SerialWrite(buffer[position]);
}

void comsiprint(long long number, int radix) {
	if (number < 0) {
		SerialWrite('-');
		comusiprint(-number, radix);
	} else comusiprint(number, radix);
}

// Print function

#define CPRINT_NORMAL_STATE 0
#define CPRINT_LENGTH_STATE 1
#define CPRINT_SHORT_LENGTH_STATE 2
#define CPRINT_LONG_LENGTH_STATE 3
#define CPRINT_SPECIFIER_STATE 4

#define CPRINT_DEFAULT_LENGTH 0
#define CPRINT_SSHORT_LENGTH 1
#define CPRINT_SHORT_LENGTH 2
#define CPRINT_LONG_LENGTH 3
#define CPRINT_LLONG_LENGTH 4

void cprint(const char* format, ...) {
	va_list arguments;
	va_start(arguments, format);

	int state = CPRINT_NORMAL_STATE;
	int length = CPRINT_DEFAULT_LENGTH;
	int radix = 10;
	bool sign = false;
	bool number = false;

	while (*format) {
		switch (state) {
		case CPRINT_NORMAL_STATE:
			switch (*format) {
			case '%':
				state = CPRINT_LENGTH_STATE;
				break;
			default:
					SerialWrite(*format);
				break;
			}
			break;

		case CPRINT_LENGTH_STATE:
			switch (*format) {
			case 'h':
				length = CPRINT_SHORT_LENGTH;
				state = CPRINT_SHORT_LENGTH_STATE;
				break;
			case 'l':
				length = CPRINT_LONG_LENGTH_STATE;
				break;
			default:
				goto CPRINT_SPECIFIER_STATE_;
			}
			break;

		case CPRINT_SHORT_LENGTH_STATE:
			if (*format == 'h') {
				length = CPRINT_SSHORT_LENGTH;
				state = CPRINT_SPECIFIER_STATE;
			} else goto CPRINT_SPECIFIER_STATE_;
			break;

		case CPRINT_LONG_LENGTH_STATE:
			if (*format == 'l') {
				length = CPRINT_LLONG_LENGTH;
				state = CPRINT_SPECIFIER_STATE;
			} else goto CPRINT_SPECIFIER_STATE_;
			break;

		case CPRINT_SPECIFIER_STATE:
		CPRINT_SPECIFIER_STATE_:
			switch(*format) {
			case 'c':
				SerialWrite((char)va_arg(arguments, int));
				break;
			case 's':
				comstrput(va_arg(arguments, const char*));
				break;
			case '%':
				SerialWrite('%');
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
					case CPRINT_SSHORT_LENGTH:
					case CPRINT_SHORT_LENGTH:
					case CPRINT_DEFAULT_LENGTH:
						comsiprint(va_arg(arguments, int), radix);
						break;

					case CPRINT_LONG_LENGTH:
						comsiprint(va_arg(arguments, long), radix);
						break;

					case CPRINT_LLONG_LENGTH:
						comsiprint(va_arg(arguments, long long), radix);
						break;
					}
				} else {
					switch (length) {
					case CPRINT_SSHORT_LENGTH:
					case CPRINT_SHORT_LENGTH:
					case CPRINT_DEFAULT_LENGTH:
						comusiprint(va_arg(arguments, unsigned int), radix);
						break;

					case CPRINT_LONG_LENGTH:
						comusiprint(va_arg(arguments, unsigned long), radix);
						break;

					case CPRINT_LLONG_LENGTH:
						comusiprint(va_arg(arguments, unsigned long long), radix);
						break;
					}
				}
			}

			state = CPRINT_NORMAL_STATE;
			length = CPRINT_DEFAULT_LENGTH;
			radix = 10;
			sign = false;
			break;
		}

		format++;
	}

	va_end(arguments);
}

void cbufferprint(const char *message, const void *buffer, uint32_t count) {
	const uint8_t* unsigned8buffer = (const uint8_t *)buffer;
	comstrput(message);
	for (uint16_t i = 0; i < count; i++) {
		SerialWrite(CHEXADECIMAL_CHARACTERS[unsigned8buffer[i] >> 4]);
		SerialWrite(CHEXADECIMAL_CHARACTERS[unsigned8buffer[i] & 0xF]);
	}
	comstrput("\r\n");
}

void comget(char *buffer, char stop) {
	uint16_t i = 0;
	while ((*buffer = SerialRead()) != stop) {
		SerialWrite(*buffer);
		buffer++;
		i++;
	}
	buffer -= i;
}
