#include "isr.h"
#include "stdio.h"
#include "comstdio.h"
#include "pic.h"
#include "x86.h"

void exceptionHandler(uint8_t exception);
void interruptHandler(uint8_t interrupt);

void KernelPanic(const char* message, uint8_t exception) {
	cprint("[PANIC] Kernel panic reached, exception code %A (0x%x)\r\n", exception, exception);
	cprint("[PANIC] %s\r\n", message);
	setDefaultColour(VGA_WHITE | VGA_MAGENTA << 4);
	clearscreen();
	print("***KERNEL PANIC***\r\nAn unhandled exception %A has occured. Stop.\r\n", exception);
	print("\r\n%s\r\n", message);
	__asm__ volatile ("cli; hlt");
}

void exceptionHandler(uint8_t exception) {
	switch (exception) {
	case EXCEPTION_DIVIDE_BY_ZERO:
		KernelPanic("You can't divide by zero.", exception);
		break;
	case EXCEPTION_PAGE:
		KernelPanic("Unexpected page fault.", exception);
		break;
	default:
		KernelPanic("Unhandled exception.", exception);
	}
}

void interruptHandler(uint8_t interrupt) {
	PICSendEndOfInterrupt(interrupt);
	uint8_t KeyboardStatus = x86Input(0x64);
	if (KeyboardStatus & 1) {
		uint8_t KeyboardScancode = x86Input(0x60);
		print("A key was pressed. Scancode is %u.\r\n", KeyboardScancode);
	}
}
