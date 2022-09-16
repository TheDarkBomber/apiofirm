#include "text.h"
#include "colours.h"
#include "maths.h"
#include "paging.h"
#include "memory.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"

extern uintptr_t _KStartLoc;
extern uintptr_t _KEndLoc;

void _start(BootInfo* boot) {
	asm ("cli");

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

	PICRemap(0x20, 0x28);
	IRQMaskAll();
	IRQClearMask(0x01);

	strput("Initialising GDT...\n");
	MetaGDT metaGDT;
	metaGDT.Size = sizeof(GDTStructure) - 1;
	metaGDT.Offset = (uint64_t)&GDT;
	LoadGDT(&metaGDT);

	printf("Initialising IDT...\n");
	InitialiseInterrupts();

  strput("Initialising paging...\n");
	InitialisePFA(boot->MMap, boot->mmapSize, boot->mmapDSize);

	uint64_t KSize = (uint64_t)&_KEndLoc - (uint64_t)&_KStartLoc;

	LockPages((char*)&_KStartLoc, U64CeilingDivision(KSize, 4096));
	printf("Total RAM: 0x%x KB\n", PageCTX.TotalMemorySize);
	printf("Free RAM: 0x%x KB\n", PageCTX.FreeMemorySize);
	printf("Used RAM: 0x%x KB\n", PageCTX.UsedMemorySize);
	printf("Reserved RAM: 0x%x KB\n", PageCTX.ReservedMemorySize);

	LockPages((char*)boot->GFX.FrameBuffer, U64CeilingDivision((uint64_t)boot->GFX.FrameBuffer + boot->GFX.Pitch * boot->GFX.Height * 4 + 0x1000, 4096));

	PML4 = (PageTable*)RequestPage();
	memset((char*)PML4, 0, 0x1000);

	for (uint64_t i = (uint64_t)boot->GFX.FrameBuffer; i < (uint64_t)boot->GFX.FrameBuffer + boot->GFX.Width * boot->GFX.Pitch * 4 + 0x1000; i += 4096)
		MapMemoryV2P((char*)i, (char*)i);

	for (uint64_t i = 0; i < PageCTX.TotalMemorySize; i += 0x1000) MapMemoryV2P((char *)i, (char *)i);

	asm ("mov %0, %%cr3" : : "r" (PML4));

	TextCTX.Foreground = SOFTGREEN;
	printf("Paging initialised.\n");
	TextCTX.Foreground = FG_BEE;

	SetStandardKeymap();

	TextCTX.Foreground = WHITE;
	printf("END OF KERNEL\n");

	while (1);
}
