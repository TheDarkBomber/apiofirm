#include "text.h"
#include "memory.h"
#include <stdarg.h>
#include <stdbool.h>

void plotPixel(VideoOut *gfx, int x, int y, unsigned long pixel);
void plotBlock(VideoOut *gfx, int x, int y, uint32_t blockSize, uint32_t colour);

TextContext TextCTX;

static void displayChar(VideoOut *gfx, char c, int x, int y, uint32_t colour, uint8_t bs, FontPSF1 *font) {
  uint8_t row;
  int xScan = x;
  for (int j = 0; j < font->Height; j++) {
    row = font->Font[c * font->Height + j];
    for (int i = 0; i < 8; i++) {
      if (row & 0x80) bs == 1 ? plotPixel(gfx, xScan, y, colour) : plotBlock(gfx, xScan, y, bs, colour);
      row = row << 1;
      xScan = xScan + bs;
    }
    y = y + bs;
    xScan = x;
  }
}

static void displayString(VideoOut *gfx, char *s, int x, int y, uint32_t colour, uint8_t bs, FontPSF1 *font) {
  int k = 0;
  while (s[k]) {
    displayChar(gfx, s[k], x, y, colour, bs, font);
    x = x + bs * 9;
    k++;
  }
}

static void clearChar(VideoOut* gfx, int x, int y, uint32_t colour, FontPSF1* font) {
	int xScan = x;
	for (int j = 0; j < font->Height; j++) {
		for (int i = 0; i < 8; i++, xScan++) plotPixel(gfx, xScan, y, colour);
		y++;
		xScan = x;
	}
}

void scroll(unsigned lines) {
	char* FB = (char*)TextCTX.GFX->FrameBuffer;
	memcpy(FB, FB + TextCTX.GFX->Pitch * 4 * lines * TextCTX.TFX->Height, TextCTX.GFX->Pitch * 4 * (TextCTX.GFX->Height - lines * TextCTX.TFX->Height));
	TextCTX.CursorY -= lines;
	ClearScreen(TextCTX.CursorY * TextCTX.TFX->Height);
}

void charput(char c) {
	switch (c) {
	case '\n':
		TextCTX.CursorY++;
	case '\r':
		TextCTX.CursorX = 0;
		break;
	case '\t':
		TextCTX.CursorX += 4 - (TextCTX.CursorX % 4);
		break;
	case '\v':
		TextCTX.CursorY += 4 - (TextCTX.CursorY % 4);
		break;
	case '\f':
		scroll(TextCTX.GFX->Height / TextCTX.TFX->Height);
		break;
	case '\b':
		if (TextCTX.CursorX == 0) {
			TextCTX.CursorY--;
			TextCTX.CursorX = (TextCTX.GFX->Width / 8) - 1;
		} else TextCTX.CursorX--;
		clearChar(TextCTX.GFX, TextCTX.CursorX * 8, TextCTX.CursorY * TextCTX.TFX->Height, TextCTX.Background, TextCTX.TFX);
		break;
	default:
		displayChar(TextCTX.GFX, c, TextCTX.CursorX * 8, TextCTX.CursorY * TextCTX.TFX->Height, TextCTX.Foreground, 1, TextCTX.TFX);
		TextCTX.CursorX++;
		break;
	}

	if (TextCTX.CursorX * 8 >= TextCTX.GFX->Width) {
		TextCTX.CursorY++;
		TextCTX.CursorX = 0;
	}

	if (TextCTX.CursorY * TextCTX.TFX->Height >= TextCTX.GFX->Height) scroll(1);
}

void strput(const char* s) {
	while (*s) {
		charput(*s);
		s++;
	}
}

void ClearScreen(int fromY) {
	for (int x = 0; x < TextCTX.GFX->Width; x++) {
		for (int y = fromY; y < TextCTX.GFX->Height; y++) {
			plotPixel(TextCTX.GFX, x, y, TextCTX.Background);
		}
	}
}

const char HEXADECIMAL_CHARACTERS[] = "0123456789ABCDEF";
const char NIFTIMAL_CHARACTERS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void usiprint(uint64_t number, int radix) {
	char buffer[32];
	int position = 0;

	do {
		uint64_t remainder = number % radix;
		number /= radix;
		buffer[position++] = NIFTIMAL_CHARACTERS[remainder];
	} while (number > 0);

	while (--position >= 0) charput(buffer[position]);
}

void siprint(int64_t number, int radix) {
	if (number < 0) {
		charput('-');
		usiprint(-number, radix);
	} else usiprint(number, radix);
}

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

void printf(const char* format, ...) {
	va_list arguments;
	va_start(arguments, format);
	printfv(format, arguments);
	va_end(arguments);
}

void printfv(const char* format, va_list arguments) {
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
}

