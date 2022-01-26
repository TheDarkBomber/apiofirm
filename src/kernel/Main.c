#include <stdint.h>
#include "stdio.h"
#include "memory.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void __attribute__((section(".entry"))) bzzzzzt(uint16_t bootLocation) {
	memset(&__bss_start, 0, (&__end) - (&__bss_start));
	setprintPosition(0, 4);
	print("[KERNEL] Bzzzzzzzzzt! Kernel loaded c:\r\n");
	setDefaultColour(VGA_LIGHT_CYAN);
	print("[KERNEL] In multi-colour.");
	for (;;);
}
