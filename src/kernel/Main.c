#include <stdint.h>
#include "idt.h"
#include "pic.h"
#include "stdio.h"
#include "memory.h"
#include "x86.h"

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

	for(;;);
}
