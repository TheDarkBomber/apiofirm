#include <stdint.h>
#include "idt.h"
#include "pic.h"
#include "stdio.h"
#include "memory.h"
#include "paging.h"
#include "x86.h"
#include "serial.h"
#include "comstdio.h"

extern uint32_t __bss_start;
extern uint32_t __end;

void __attribute__((section(".entry"))) bzzzzzt(uint16_t bootLocation) {
	setprintPosition(0, 4);
	print("[KERNEL] Bzzzzzzzzzt! Kernel loaded c:\r\n");
	setDefaultColour(VGA_LIGHT_CYAN);
	print("[KERNEL] In multi-colour.\r\n");
	setDefaultColour(VGA_YELLOW);

	PICRemap(0x20, 0x28);
	IRQMaskAll();
	IRQClearMask(0x01);
	print("[KERNEL] Remapped PIC.\r\n");

	InitialiseIDT();
	print("[KERNEL] Initialised IDT.\r\n");

	__asm__ volatile ("sti");
	if (!InitialiseSerial()) comstrput("[KERNEL] Initialised serial.\r\n");

	InitialisePaging();
	print("[KERNEL] Initialised paging.\r\n");
	cprint("[KERNEL] Page directory located at %x\r\n", x86ReadCR3());

	for(;;);
}
