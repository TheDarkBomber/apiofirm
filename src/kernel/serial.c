#include "serial.h"
#include "x86.h"
#include <stdbool.h>

#define COM1 0x3F8

static bool InitialiseSerial() {
	x86Output(COM1 + 1, 0x00);
	x86Output(COM1 + 3, 0x80);
	x86Output(COM1 + 0, 0x03);
	x86Output(COM1 + 1, 0x00);
	x86Output(COM1 + 3, 0x03);
	x86Output(COM1 + 2, 0xC7);
	x86Output(COM1 + 4, 0x0B);
	x86Output(COM1 + 4, 0x1E);
	x86Output(COM1 + 0, 0xAE);

	if (x86Input(COM1) != 0xAE) return true;

	x86Output(COM1 + 4, 0x0F);
	return false;
}

int SerialReceived() { return x86Input(COM1 + 5) & 1; }
int TransmitEmpty() { return x86Input(COM1 + 5) & 0x20; }

char SerialRead() {
	while (SerialReceived() == 0);
	return x86Input(COM1);
}

void SerialWrite(char c) {
	while (TransmitEmpty() == 0);
	x86Output(COM1, c);
}
