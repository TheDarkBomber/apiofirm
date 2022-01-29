#ifndef __IDT_IDT_KERNEL_H
#define __IDT_IDT_KERNEL_H
#include <stdint.h>

typedef struct {
	uint16_t ISR_LOW;
	uint16_t KERNEL_CODE_SELECTOR;
	uint8_t RSVP;
	uint8_t ATTRIBUTES;
	uint16_t ISR_HIGH;
} __attribute__((packed)) IDTEntry;

void IDTSetDescriptor(uint8_t vector, void* ISR, uint8_t flags);
void InitialiseIDT();

#endif
