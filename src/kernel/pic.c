#include "pic.h"
#include "x86.h"

#define IO_WAIT 0x80

void PICRemap(int offset1, int offset2) {
	unsigned char argument1, argument2;

	argument1 = x86Input(PIC_WORKER_DATA);
	argument2 = x86Input(PIC_DRONE_DATA);

	x86Output(PIC_WORKER_COMMAND, ICW1_INITIALISE | ICW1_ICW4);
	x86Output(IO_WAIT, 0);

	x86Output(PIC_DRONE_COMMAND, ICW1_INITIALISE | ICW1_ICW4);
	x86Output(IO_WAIT, 0);

	x86Output(PIC_WORKER_DATA, offset1);
	x86Output(IO_WAIT, 0);

	x86Output(PIC_DRONE_DATA, offset2);
	x86Output(IO_WAIT, 0);

	x86Output(PIC_WORKER_DATA, 4);
	x86Output(IO_WAIT, 0);

	x86Output(PIC_DRONE_DATA, 2);
	x86Output(IO_WAIT, 0);

	x86Output(PIC_WORKER_DATA, ICW4_8086);
	x86Output(IO_WAIT, 0);

	x86Output(PIC_DRONE_DATA, ICW4_8086);
	x86Output(IO_WAIT, 0);

	x86Output(PIC_WORKER_DATA, argument1);
	x86Output(PIC_DRONE_DATA, argument2);
}

void PICSendEndOfInterrupt(unsigned char interruptRequest) {
	if (interruptRequest >= 8) x86Output(PIC_DRONE_COMMAND, PIC_END_OF_INTERRUPT);
	x86Output(PIC_WORKER_COMMAND, PIC_END_OF_INTERRUPT);
}

static uint16_t PICGetInterruptRequestRegister(int OCW3) {
	x86Output(PIC_WORKER_COMMAND, OCW3);
	x86Output(PIC_DRONE_COMMAND, OCW3);
	return (x86Input(PIC_DRONE_COMMAND) << 8) | x86Input(PIC_DRONE_COMMAND);
}

uint16_t PICGetIRR(void) { return PICGetInterruptRequestRegister(PIC_IRR); }
uint16_t PICGetISR(void) { return PICGetInterruptRequestRegister(PIC_ISR); }

void IRQSetMask(uint8_t IRQ) {
	uint16_t port;
	uint8_t value;

	if (IRQ < 8) port = PIC_WORKER_DATA;
	else {
		port = PIC_DRONE_DATA;
		IRQ -= 8;
	}

	value = x86Input(port) | (1 << IRQ);
	x86Output(port, value);
}

void IRQClearMask(uint8_t IRQ) {
	uint16_t port;
	uint8_t value;

	if (IRQ < 8) port = PIC_WORKER_DATA;
	else {
		port = PIC_DRONE_DATA;
		IRQ -= 8;
	}

	value = x86Input(port) & ~(1 << IRQ);
	x86Output(port, value);
}

void IRQMaskAll() {
	x86Output(PIC_WORKER_DATA, 0xFF);
	x86Output(PIC_DRONE_DATA, 0xFF);
}
