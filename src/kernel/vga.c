#include "vga.h"
#include "x86.h"
#include <stdint.h>

void VGAWrite(uint16_t port, uint8_t VGAregister, uint8_t value) {
	x86Output(port, VGAregister);
	x86Output(port + 1, value);
}

uint8_t VGARead(uint16_t port, uint8_t VGAregister) {
	x86Output(port, VGAregister);
	return x86Input(port + 1);
}

void VGASetFont(uint8_t font[4096]) {
	uint8_t *video = (uint8_t*)0xB8000;
	uint8_t memoryMode, graphicsMode;

	VGAWrite(VGA_SEQUENCE_INDEX, VGA_SEQUENCE_MAP_MASK, 0x04);
	VGAWrite(VGA_SEQUENCE_INDEX, VGA_SEQUENCE_CHARACTER_SET, 0x00);

	memoryMode = VGARead(VGA_SEQUENCE_INDEX, VGA_SEQUENCE_MEMORY_MODE);
	VGAWrite(VGA_SEQUENCE_INDEX, VGA_SEQUENCE_MEMORY_MODE, 0x06);
	VGAWrite(VGA_GC_INDEX, VGA_GC_READ_MAP, 0x02);

	graphicsMode = VGARead(VGA_GC_INDEX, VGA_GC_GRAPHICS_MODE);
	VGAWrite(VGA_GC_INDEX, VGA_GC_GRAPHICS_MODE, 0x00);
	VGAWrite(VGA_GC_INDEX, VGA_GC_GENERAL, 0x0C);

	for (uint16_t i = 0; i < 256; i++) {
		for(uint8_t j = 0; j < 16; j++) {
			*video = *font;
			++video; ++font;
		}
		video += 16;
	}

	VGAWrite(VGA_SEQUENCE_INDEX, VGA_SEQUENCE_MAP_MASK, 0x03);
	VGAWrite(VGA_SEQUENCE_INDEX, VGA_SEQUENCE_MEMORY_MODE, memoryMode);
	VGAWrite(VGA_GC_INDEX, VGA_GC_READ_MAP, 0x00);
	VGAWrite(VGA_GC_INDEX, VGA_GC_GRAPHICS_MODE, graphicsMode);
	VGAWrite(VGA_GC_INDEX, VGA_GC_GENERAL, 0x0C);
}
