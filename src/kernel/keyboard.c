#include "keyboard.h"
#include "text.h"
#include "pic.h"
#include "x86.h"
#include <stdbool.h>

static char NormalKeymap[256];
static char ExtendedKeymap[256];
static char ShiftKeymap[256];

static bool MagicKeyActive = false;
static bool ExtendedMode = false;
static bool ShiftMode = false;

static void kprint(char scancode) {
	if (ExtendedMode) {
		if (ExtendedKeymap[scancode]) charput(ExtendedKeymap[scancode]);
	} else {
		if (ShiftMode && ShiftKeymap[scancode]) charput(ShiftKeymap[scancode]);
		else if (NormalKeymap[scancode]) charput(NormalKeymap[scancode]);
	}
}

static void handleMagicKey() {
	MagicKeyActive = !MagicKeyActive;
	if (MagicKeyActive) asm("int $0x03");
}

void KeyboardHandler(uint64_t irq) {
	PICSendEndOfInterrupt(irq);
	char kbStatus = x86Input(0x64);
	if (kbStatus & 1) {
		unsigned char kbScancode = x86Input(0x60);
		if (kbScancode == KEY_MAGIC) handleMagicKey();
		if (!ExtendedMode && kbScancode == KEY_EXTENDED) {
			ExtendedMode = true;
			return;
		}
		if (kbScancode == KEY_SHIFT) ShiftMode = true;
		else if (kbScancode == KEY_SHIFT + KEY_RELEASE_OFFSET) ShiftMode = false;
		kprint(kbScancode);
		ExtendedMode = false;
	}
}

void SetStandardKeymap() {
	for (int i = 0; i < 256; i++) {
		NormalKeymap[i] = 0;
		ExtendedKeymap[i] = 0;
	}

	NormalKeymap[KEY_A] = 'a';
	NormalKeymap[KEY_B] = 'b';
	NormalKeymap[KEY_C] = 'c';
	NormalKeymap[KEY_D] = 'd';
	NormalKeymap[KEY_E] = 'e';
	NormalKeymap[KEY_F] = 'f';
	NormalKeymap[KEY_G] = 'g';
	NormalKeymap[KEY_H] = 'h';
	NormalKeymap[KEY_I] = 'i';
	NormalKeymap[KEY_J] = 'j';
	NormalKeymap[KEY_K] = 'k';
	NormalKeymap[KEY_L] = 'l';
	NormalKeymap[KEY_M] = 'm';
	NormalKeymap[KEY_N] = 'n';
	NormalKeymap[KEY_O] = 'o';
	NormalKeymap[KEY_P] = 'p';
	NormalKeymap[KEY_Q] = 'q';
	NormalKeymap[KEY_R] = 'r';
	NormalKeymap[KEY_S] = 's';
	NormalKeymap[KEY_T] = 't';
	NormalKeymap[KEY_U] = 'u';
	NormalKeymap[KEY_V] = 'v';
	NormalKeymap[KEY_W] = 'w';
	NormalKeymap[KEY_X] = 'x';
	NormalKeymap[KEY_Y] = 'y';
	NormalKeymap[KEY_Z] = 'z';

	NormalKeymap[KEY_0] = '0';
	NormalKeymap[KEY_1] = '1';
	NormalKeymap[KEY_2] = '2';
	NormalKeymap[KEY_3] = '3';
	NormalKeymap[KEY_4] = '4';
	NormalKeymap[KEY_5] = '5';
	NormalKeymap[KEY_6] = '6';
	NormalKeymap[KEY_7] = '7';
	NormalKeymap[KEY_8] = '8';
	NormalKeymap[KEY_9] = '9';

	NormalKeymap[KEY_FORWARDSLASH] = '/';
	NormalKeymap[KEY_BACKSLASH] = '\\';
	NormalKeymap[KEY_APOSTROPHE] = '\'';
	NormalKeymap[KEY_SCLN] = ';';
	NormalKeymap[KEY_COMMA] = ',';
	NormalKeymap[KEY_DOT] = '.';
	NormalKeymap[KEY_DASH] = '-';
	NormalKeymap[KEY_EQUALS] = '=';
	NormalKeymap[KEY_BACKTICK] = '`';
	NormalKeymap[KEY_OCTOTHORPE] = '#';
	NormalKeymap[KEY_LSQP] = '[';
	NormalKeymap[KEY_RSQP] = ']';

	NormalKeymap[KEY_SPACE] = ' ';
	NormalKeymap[KEY_TAB] = '\t';
	NormalKeymap[KEY_ENTER] = '\n';
	NormalKeymap[KEY_BKSP] = '\b';

	NormalKeymap[KEY_K0] = '0';
	NormalKeymap[KEY_K1] = '1';
	NormalKeymap[KEY_K2] = '2';
	NormalKeymap[KEY_K3] = '3';
	NormalKeymap[KEY_K4] = '4';
	NormalKeymap[KEY_K5] = '5';
	NormalKeymap[KEY_K6] = '6';
	NormalKeymap[KEY_K7] = '7';
	NormalKeymap[KEY_K8] = '8';
	NormalKeymap[KEY_K9] = '9';

	ExtendedKeymap[KEY_ADD] = '+';
	ExtendedKeymap[KEY_MINUS] = '-';
	ExtendedKeymap[KEY_FRACTIONAL_POINT] = '.';
	ExtendedKeymap[KEY_KFORWARDSLASH] = '/';
	ExtendedKeymap[KEY_KENTER] = '\n';

	for (int i = 0; i < 256; i++) ShiftKeymap[i] = NormalKeymap[i];

	ShiftKeymap[KEY_A] = 'A';
	ShiftKeymap[KEY_B] = 'B';
	ShiftKeymap[KEY_C] = 'C';
	ShiftKeymap[KEY_D] = 'D';
	ShiftKeymap[KEY_E] = 'E';
	ShiftKeymap[KEY_F] = 'F';
	ShiftKeymap[KEY_G] = 'G';
	ShiftKeymap[KEY_H] = 'H';
	ShiftKeymap[KEY_I] = 'I';
	ShiftKeymap[KEY_J] = 'J';
	ShiftKeymap[KEY_K] = 'K';
	ShiftKeymap[KEY_L] = 'L';
	ShiftKeymap[KEY_M] = 'M';
	ShiftKeymap[KEY_N] = 'N';
	ShiftKeymap[KEY_O] = 'O';
	ShiftKeymap[KEY_P] = 'P';
	ShiftKeymap[KEY_Q] = 'Q';
	ShiftKeymap[KEY_R] = 'R';
	ShiftKeymap[KEY_S] = 'S';
	ShiftKeymap[KEY_T] = 'T';
	ShiftKeymap[KEY_U] = 'U';
	ShiftKeymap[KEY_V] = 'V';
	ShiftKeymap[KEY_W] = 'W';
	ShiftKeymap[KEY_X] = 'X';
	ShiftKeymap[KEY_Y] = 'Y';
	ShiftKeymap[KEY_Z] = 'Z';

	// '('
	ShiftKeymap[KEY_0] = ')';
	ShiftKeymap[KEY_1] = '!';
	ShiftKeymap[KEY_2] = '"';
	ShiftKeymap[KEY_3] = '\xD1';
	ShiftKeymap[KEY_4] = '$';
	ShiftKeymap[KEY_5] = '%';
	ShiftKeymap[KEY_6] = '^';
	ShiftKeymap[KEY_7] = '&';
	ShiftKeymap[KEY_8] = '*';
	ShiftKeymap[KEY_9] = '(';
	// ')'

	ShiftKeymap[KEY_FORWARDSLASH] = '?';
	ShiftKeymap[KEY_BACKSLASH] = '|';
	ShiftKeymap[KEY_APOSTROPHE] = '@';
	ShiftKeymap[KEY_SCLN] = ':';
	ShiftKeymap[KEY_COMMA] = '<';
	ShiftKeymap[KEY_DOT] = '>';
	ShiftKeymap[KEY_DASH] = '_';
	ShiftKeymap[KEY_EQUALS] = '+';
	ShiftKeymap[KEY_BACKTICK] = '|';
	ShiftKeymap[KEY_OCTOTHORPE] = '~';
	ShiftKeymap[KEY_LSQP] = '{';
	ShiftKeymap[KEY_RSQP] = '}';

	ShiftKeymap[KEY_TAB] = '\v';
	ShiftKeymap[KEY_ENTER] = '\r';
}
