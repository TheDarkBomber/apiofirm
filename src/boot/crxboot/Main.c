#include "stdint.h"
#include "stdio.h"

void _cdecl cstart_(uint16_t bootLocation) {
	strput("[CRXBOOT] Loaded second stage bootloader. Bzzzzzt.\r\n");
	print("[CRXBOOT] CRXBOOT located on drive %d\r\n", bootLocation);
	print("[CRXBOOT] 0x%x = 0o%o = %d !\r\n", 0x0BEE, 0x0BEE, 0x0BEE);
	print("[CRXBOOT] In base 6, 0x%x = %a\r\n", 0x0BEE, 0x0BEE);
	print("[CRXBOOT] In niftimal, that's %A\r\n", 0x0BEE);
	for (;;);
}
