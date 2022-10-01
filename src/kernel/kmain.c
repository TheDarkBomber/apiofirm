#include <hci/text.h>
#include <colours.h>
#include <maths.h>
#include <memory/paging.h>
#include <memory.h>
#include <x86/gdt.h>
#include <int/idt.h>
#include <int/pic.h>
#include <int/keyboard.h>
#include <acpi/acpi.h>
#include <pci/ahci.h>
#include <pci/pci.h>
#include <memory/heap.h>
#include <int/pit.h>
#include <kattrs.h>
#include <disk/gpt.h>
#include <disk/fat.h>
#include <disk/disk.h>
#include <hci/serial.h>
#include <lai/helpers/sci.h>
#include <stddef.h>

extern uintptr_t _KStartLoc;
extern uintptr_t _KEndLoc;

void _start(BootInfo* boot) {
	asm ("cli");

	ApiofirmCTX.EOI = 1;

	TextCTX.GFX = &boot->GFX;
	TextCTX.TFX = &boot->TFX;
	TextCTX.CursorX = 0;
	TextCTX.CursorY = 0;
	TextCTX.Foreground = FG_BEE;
	TextCTX.Background = BG_BEE;

	InitialiseSerial();

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
	prints("\x1B[32m");
	prints("Total RAM: 0x%x KB\n", PageCTX.TotalMemorySize);
	prints("Free RAM: 0x%x KB\n", PageCTX.FreeMemorySize);
	prints("Used RAM: 0x%x KB\n", PageCTX.UsedMemorySize);
	prints("Reserved RAM: 0x%x KB\n", PageCTX.ReservedMemorySize);
	prints("\x1B[0m");

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

	InitialiseHeap((char *)0x000010000000000, 16);
	printf("Initialised heap at address 0x%X\n", HeapCTX.Start);

	SystemDescriptorTable* XSDT = (SystemDescriptorTable*)(boot->RSDP->XSDTAddress);
	ACPISetXSDT(XSDT);
	lai_set_acpi_revision(boot->RSDP->Revision);
	lai_create_namespace();
	lai_enable_acpi(0);

	MCFGHeader* MCFG = (MCFGHeader*)ACPIFindTable(XSDT, "MCFG");
	if (MCFG) {
		prints("\x1B[34m[ACPI] Found MCFG at 0x%x\n\x1B[0m", MCFG);
		InitialisePCI(MCFG);
	} else {
		TextCTX.Foreground = SOFTRED;
		printf("WARNING: Failed to find MCFG in ACPI tables. Without this, PCI functionality will not be available.\n");
		prints("\x1B[1;31m[ACPI] Failed to find MCFG. PCI functionality disabled.\n\x1B[0m");
		TextCTX.Foreground = FG_BEE;
	}

	/* InitialisePIT(); */
	/* IRQClearMask(0x00); */ // PIT fundamentally incompatible with AHCI‽

	TextCTX.Foreground = WHITE;
	printf("END OF KERNEL\n");

	while (1);
}
