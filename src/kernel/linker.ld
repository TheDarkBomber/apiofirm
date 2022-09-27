OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS {
	_KStartLoc = .;
	.text : ALIGN(0x1000) { *(.text) }
	.data : ALIGN(0x1000) { *(.data) }
	.rodata : ALIGN(0x1000) { *(.rodata) }
	.bss : ALIGN(0x1000) { *(COMMON) *(.bss) }
	_KEndLoc = .;
}

laihost_malloc = mallocate;
laihost_realloc = mreallocate;
laihost_free = mfree;
laihost_outb = x86Output;
laihost_inb = x86Input;
laihost_outw = x86OutputWide;
laihost_inw = x86InputWide;
laihost_outd = x86OutputDouble;
laihost_ind = x86InputDouble;
