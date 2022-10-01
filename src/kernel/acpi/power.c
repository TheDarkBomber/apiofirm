#include <acpi/power.h>
#include <panic.h>
#include <int/idt.h>
#include <memory.h>
#include <lai/helpers/pm.h>

void PowerOff() {
	lai_enter_sleep(5);
	GenericKernelPanic("Failed to power off system.", "POWER SUCCESS");
}

void ResetComputer() {
	lai_acpi_reset();
	// ACPI failed, resort to triple faulting!
	asm("cli");

	MetaIDT NULL_IDT;
	memset((char*)&NULL_IDT, 0, sizeof(MetaIDT));

	asm("lidt %0" : : "m"(NULL_IDT));
	asm("sti");
	asm("int $0x03"); // just to be sure
}
