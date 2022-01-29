#ifndef __PIC_ISR_KERNEL
#define __PIC_ISR_KERNEL

#include <stdint.h>

#define PIC_WORKER 0x20
#define PIC_DRONE 0xA0

#define PIC_WORKER_COMMAND 0x20
#define PIC_WORKER_DATA 0x21
#define PIC_DRONE_COMMAND 0xA0
#define PIC_DRONE_DATA 0xA1

#define PIC_IRR 0x0A
#define PIC_ISR 0x0B

#define PIC_END_OF_INTERRUPT 0x20

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INITIALISE 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUFFER_DRONE 0x08
#define ICW4_BUFFER_WORKER 0x0C
#define ICW4_SFNM 0x10

void PICRemap(int offset1, int offset2);
void PICSendEndOfInterrupt(unsigned char interruptRequest);

uint16_t PICGetIRR(void);
uint16_t PICGetISR(void);

void IRQSetMask(uint8_t IRQ);
void IRQClearMask(uint8_t IRQ);
void IRQMaskAll();

#endif
