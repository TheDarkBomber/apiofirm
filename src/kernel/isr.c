#include "isr.h"
#include "panic.h"
#include "text.h"
#include "keyboard.h"

char* ISR_Strings[0x20] = {
	"Divide by Zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Overflow",
	"Bound range exceeded",
	"Invalid opcode",
	"Device not available",
	"Double fault",
	"Coprocessor segment overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack-segment fault",
	"General protection fault",
	"Page fault",
	0,
	"x87 Floating Point Exception",
	"Alignment check",
	"Machine check",
	"SIMD",
	"Virtualisation exception",
	"Control protection exception",
	0, 0, 0, 0, 0, 0,
	"Hypervisor injection exception",
	"VMM communication exception",
	"Security exception",
	0
};

void InterruptHandler(uint64_t irq) {
	switch (irq) {
	case ISR_Page:
		GenericKernelPanic("A page fault has occurred.", ISR_Strings[irq]);
		break;
	case ISR_DoubleFault:
		GenericKernelPanic("A double fault has occurred. Abort.", ISR_Strings[irq]);
		break;
	case ISR_Keyboard:
		KeyboardHandler(irq);
		break;
	default:
		if (irq <= 20)GenericKernelPanic("An unknown fault 0x%x has occurred.", ISR_Strings[irq], irq);
		break;
	}
}
