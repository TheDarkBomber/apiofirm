#include "stdio.h"
#include "x86.h"

void charput(char c) { x86TeletypeModeWriteCharacter(c, 0); }

void strput(const char* s) {
	while (*s) {
		charput(*s);
		s++;
	}
}

void farstrput(const char far* s) {
	while (*s) {
		charput(*s);
		s++;
	}
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

int* imprint(int* argp, int length, bool sign, int radix);

void _cdecl print(const char* format, ...) {
	int* argp = (int*)&format;
	int state = PRINT_NORMAL_STATE;
	int length = PRINT_DEFAULT_LENGTH;
	int radix = 10;
	bool sign = false;

	argp++;

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
				charput((char)*argp);
				argp++;
				break;
			case 's':
				if (length == PRINT_LONG_LENGTH || length == PRINT_LLONG_LENGTH) {
					farstrput(*(const char far**)argp);
					argp += 2;
				} else {
					strput(*(const char**)argp);
					argp++;
				}
				break;
			case '%':
				charput('%');
				break;
			case 'd':
			case 'i':
				radix = 10;
				sign = true;
				argp = imprint(argp, length, sign, radix);
				break;
			case 'u':
				radix = 10;
				sign = false;
				argp = imprint(argp, length, sign, radix);
			case 'X':
			case 'x':
			case 'p':
				radix = 16;
				sign = false;
				argp = imprint(argp, length, sign, radix);
				break;
			case 'o':
				radix = 8;
				sign = false;
				argp = imprint(argp, length, sign, radix);
				break;
			case 'a':
				radix = 6;
				sign = false;
				argp = imprint(argp, length, sign, radix);
				break;
			case 'A':
				radix = 36;
				sign = false;
				argp = imprint(argp, length, sign, radix);
				break;
			default: break;
			}

			state = PRINT_NORMAL_STATE;
			length = PRINT_DEFAULT_LENGTH;
			radix = 10;
			sign = false;
			break;
		}

		format++;
	}
}

const char HEXADECIMAL_CHARACTERS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int* imprint(int* argp, int length, bool sign, int radix) {
	char buffer[32];
	unsigned long long number;
	int signOfNumber = 1;
	int position = 0;

	switch (length) {
	case PRINT_SSHORT_LENGTH:
	case PRINT_SHORT_LENGTH:
	case PRINT_DEFAULT_LENGTH:
		if (sign) {
			int n = *argp;
			if (n < 0) {
				n = -n;
				signOfNumber = -1;
			}
			number = (unsigned long long)n;
		} else number = *(unsigned int*)argp;
		argp++;
		break;

	case PRINT_LONG_LENGTH:
		if (sign) {
			long int n = *(long int*)argp;
			if (n < 0) {
				n = -n;
				signOfNumber = -1;
			}
			number = (unsigned long long)n;
		} else number = *(unsigned long int*)argp;
		argp += 2;
		break;

	case PRINT_LLONG_LENGTH:
		if (sign) {
			long long int n = *(long long int*)argp;
			if (n < 0) {
				n = -n;
				signOfNumber = -1;
			}
			number = (unsigned long long)n;
		} else number = *(unsigned long long int*)argp;
		argp += 4;
		break;
	}

	do {
		uint32_t remainder;
		x86Divide64By32(number, radix, &number, &remainder);
		buffer[position++] = HEXADECIMAL_CHARACTERS[remainder];
	} while (number > 0);

	if (sign && signOfNumber < 0) buffer[position++] = '-';

	while (--position >= 0) charput(buffer[position]);
	return argp;
}
