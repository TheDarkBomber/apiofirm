#include "text.h"
#include "colours.h"
#include "maths.h"
#include "paging.h"

extern uintptr_t _KStartLoc;
extern uintptr_t _KEndLoc;

void _start(BootInfo* boot) {
	TextCTX.GFX = &boot->GFX;
	TextCTX.TFX = &boot->TFX;
	TextCTX.CursorX = 0;
	TextCTX.CursorY = 0;
	TextCTX.Foreground = FG_BEE;
	TextCTX.Background = BG_BEE;

	ClearScreen(0);
	strput("WELCOME TO APIOFIRM!\n");
	printf("Pitch is %u\nWidth is %u\nHeight is %u\n", boot->GFX.Pitch, boot->GFX.Width, boot->GFX.Height);
	printf("Font height is 0x%x which is %u which is %a!\n", boot->TFX.Height, boot->TFX.Height, boot->TFX.Height);

	strput("Initialising paging\n");
	InitialisePFA(boot->MMap, boot->mmapSize, boot->mmapDSize);
	strput("Paging initialised!\n");

	uint64_t KSize = (uint64_t)&_KEndLoc - (uint64_t)&_KStartLoc;

	LockPages((char*)&_KStartLoc, U64CeilingDivision(KSize, 4096));
	strput("Pages locked for kernel use.\n");
	printf("Total RAM: 0x%x KB\n", PageCTX.TotalMemorySize);
	printf("Free RAM: 0x%x KB\n", PageCTX.FreeMemorySize);
	printf("Used RAM: 0x%x KB\n", PageCTX.UsedMemorySize);
	printf("Reserved RAM: 0x%x KB\n", PageCTX.ReservedMemorySize);

	TextCTX.Foreground = WHITE;
	printf("END OF KERNEL\n");

	while (1);
}
