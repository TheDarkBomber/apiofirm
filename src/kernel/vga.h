#ifndef __VGA_KERNEL_H
#define __VGA_KERNEL_H
#include <stdint.h>

#define VGA_SEQUENCE_INDEX 0x3C4
#define VGA_SEQUENCE_DATA 0x3C5

#define VGA_GC_INDEX 0x3CE
#define VGA_GC_DATA 0x3CF

#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA 0x3D5

#define VGA_SEQUENCE_MAP_MASK 0x02
#define VGA_SEQUENCE_CHARACTER_SET 0x03
#define VGA_SEQUENCE_MEMORY_MODE 0x04

#define VGA_GC_READ_MAP 0x04
#define VGA_GC_GRAPHICS_MODE 0x05
#define VGA_GC_GENERAL 0x06

void VGAWrite(uint16_t port, uint8_t VGAregister, uint8_t value);
uint8_t VGARead(uint16_t port, uint8_t VGAregister);

void VGASetFont(uint8_t font[4096]);

#endif
