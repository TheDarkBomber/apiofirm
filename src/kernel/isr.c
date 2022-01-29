#include "stdio.h"
#include "pic.h"
#include "x86.h"
__attribute__((noreturn)) void exceptionHandler(void);
void interruptHandler(uint8_t interrupt);

void exceptionHandler() {
	setDefaultColour(VGA_WHITE | VGA_MAGENTA << 4);
	/* clearscreen(); */
	print("Unhandled exception.");
	__asm__ volatile ("cli; hlt");
}

void interruptHandler(uint8_t interrupt) {
	PICSendEndOfInterrupt(interrupt);
	uint8_t KeyboardStatus = x86Input(0x64);
	if (KeyboardStatus & 1) {
		uint8_t KeyboardScancode = x86Input(0x60);
		print("A key was pressed. Scancode is %u.\r\n", KeyboardScancode);
	}
}
