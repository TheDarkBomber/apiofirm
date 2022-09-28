#include "serial.h"
#include "x86.h"

bool InitialiseSerial() {
	x86Output(COM1Port + 1, 0x00);
	x86Output(COM1Port + 3, 0x80);
	x86Output(COM1Port + 0, 0x03);
	x86Output(COM1Port + 1, 0x00);
	x86Output(COM1Port + 1, 0x03);
	x86Output(COM1Port + 3, 0x03);
	x86Output(COM1Port + 2, 0xC7);
	x86Output(COM1Port + 4, 0x0B);
	x86Output(COM1Port + 4, 0x1E);
	x86Output(COM1Port + 0, 0xAE);

	if (x86Input(COM1Port) != 0xAE) return false;

	x86Output(COM1Port + 4, 0x0F);
	return true;
}

unsigned SerialTransmissionEmpty() {
	return x86Input(COM1Port + 5) & 0x20;
}

void SerialWrite(char c) {
	while (!SerialTransmissionEmpty());
	x86Output(COM1Port, c);
	if (c == '\n') SerialWrite('\r');
}

void SerialWriteString(char* s) {
	while (*s) {
		SerialWrite(*s);
		s++;
	}
}

