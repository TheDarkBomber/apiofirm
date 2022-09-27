#include "isr.h"
#include "panic.h"
#include "text.h"
#include "keyboard.h"
#include "pit.h"
#include "power.h"

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

void InterruptHandler(InterruptStack* stack) {
	switch (stack->Interrupt) {
	case ISR_Breakpoint:
		PowerOff();
	case ISR_Page:
		GenericKernelPanic("A page fault has occurred.", ISR_Strings[stack->Interrupt]);
		break;
	case ISR_DoubleFault:
		GenericKernelPanic("A double fault has occurred. Abort.", ISR_Strings[stack->Interrupt]);
		break;
	case ISR_Keyboard:
		KeyboardHandler(stack->Interrupt - 0x20);
		break;
	case ISR_Timer:
		PITHandler();
		break;
	default:
		if (stack->Interrupt <= 20) GenericKernelPanic("An unknown fault 0x%x has occurred.", ISR_Strings[stack->Interrupt], stack->Interrupt);
		break;
	}
}
