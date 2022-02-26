#include "isr.h"
#include "stdio.h"
#include "comstdio.h"
#include "string.h"
#include "pic.h"
#include "x86.h"

#define KSERIALDBGCMD(command) strcmp(command, (const char*)buffer) > 0

void exceptionHandler(uint8_t exception);
void interruptHandler(uint8_t interrupt);
void handleBreakpoint();

void KernelSerialDebug() {
	comstrput("Kernel Serial Debugger.\r\n");
	comstrput("Type `help` to get a list of commands.\r\n");
	char pbuffer[256];
	char buffer[256];
	for (;;) {
		strcpy(pbuffer, (const char*)buffer);
		memset(buffer, 0, sizeof(buffer));
		comstrput("DEBUG: ");
		comget(buffer, 0x0D);
		comstrput("\r\n");
		if (KSERIALDBGCMD("exit\r")) return;
		else if (KSERIALDBGCMD("help\r")) {
			comstrput("Kernel Serial Debugger Commands:\r\n");
			comstrput("help - helps you.\r\n");
			comstrput("bee - check if working. (interrupts interfere with comget)\r\n");
			comstrput("comget_dump - dumps buffer of what you typed last.\r\n");
			comstrput("exit - exit the Kernel Serial Debugger.\r\n");
			comstrput("panic - initiates a Kernel Panic.\r\n");
			comstrput("sti - enable interrupts.\r\n");
			comstrput("cli - disable interrupts.\r\n");
			comstrput("cr2 - read CR2 register.\r\n");
			comstrput("halt - halt the CPU.\r\n");
		} else if (KSERIALDBGCMD("panic\r")) KernelPanic("Manually initiated panic.", 0xFF);
		else if (KSERIALDBGCMD("sti\r")) __asm__ volatile ("sti");
		else if (KSERIALDBGCMD("cli\r")) __asm__ volatile ("cli");
		else if (KSERIALDBGCMD("halt\r")) __asm__ volatile ("hlt");
		else if (KSERIALDBGCMD("bee\r")) comstrput("Bzzzzzzzzzzzt.\r\n");
		else if (KSERIALDBGCMD("comget_dump\r")) cbufferprint("COMGET: ", pbuffer, strlen(pbuffer));
		else if (KSERIALDBGCMD("cr2\r")) cprint("CR2 = 0x%x\r\n", x86ReadCR2());
	}
}

void KernelPanic(const char* message, uint8_t exception) {
	cprint("[PANIC] Kernel panic reached, exception code %A (0x%x)\r\n", exception, exception);
	cprint("[PANIC] %s\r\n", message);
	setDefaultColour(VGA_WHITE | VGA_MAGENTA << 4);
	clearscreen();
	print("***KERNEL PANIC***\r\nAn unhandled exception %A has occured. Stop.\r\n", exception);
	print("\r\n%s\r\n", message);
	comstrput("Entering Kernel Serial Debugger...\r\n");
	__asm__ volatile ("cli");
	KernelSerialDebug();
	__asm__ volatile ("hlt");
}

void exceptionHandler(uint8_t exception) {
	switch (exception) {
	case EXCEPTION_DIVIDE_BY_ZERO:
		KernelPanic("You can't divide by zero.", exception);
		break;
	case EXCEPTION_BREAKPOINT:
		handleBreakpoint();
		break;
	case EXCEPTION_PAGE:
		cprint("[PAGE FAULT] Page fault occurred at address 0x%x.\r\n", x86ReadCR2());
		KernelPanic("Unexpected page fault.", exception);
		break;
	case EXCEPTION_INVALID_OPCODE:
		KernelPanic("Invalid opcode.", exception);
		break;
	default:
		KernelPanic("Unhandled exception.", exception);
	}
}

void interruptHandler(uint8_t interrupt) {
	PICSendEndOfInterrupt(interrupt);
}

void handleBreakpoint() {
	cprint("[KERNEL] Breakpoint triggered.\r\n");
	KernelSerialDebug();
}
