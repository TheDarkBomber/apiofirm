#include "isr.h"
#include "stdio.h"
#include "pic.h"
#include "x86.h"

void exceptionHandler(uint8_t exception);
void interruptHandler(uint8_t interrupt);

void KernelPanic(const char* message, uint8_t exception) {
	setDefaultColour(VGA_WHITE | VGA_MAGENTA << 4);
	clearscreen();
	print("***KERNEL PANIC***\r\nAn unhandled exception %A has occured. Stop.\r\n", exception);
	print("\r\n%s\r\n", message);
	__asm__ volatile ("cli; hlt");
}

void exceptionHandler(uint8_t exception) {
	if (exception == EXCEPTION_DIVIDE_BY_ZERO)
		KernelPanic("You can't divide by zero.\r\n", exception);
	else
		KernelPanic("Unhandled exception.\r\n", exception);
}

void interruptHandler(uint8_t interrupt) {
	PICSendEndOfInterrupt(interrupt);
	uint8_t KeyboardStatus = x86Input(0x64);
	if (KeyboardStatus & 1) {
		uint8_t KeyboardScancode = x86Input(0x60);
		print("A key was pressed. Scancode is %u.\r\n", KeyboardScancode);
	}
}
