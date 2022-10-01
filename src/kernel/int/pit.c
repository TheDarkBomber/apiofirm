#include <int/pit.h>
#include <int/pic.h>
#include <x86/x86.h>

static uint64_t sleepclock = 0;

void InitialisePIT() {
	x86Output(0x40, (uint8_t)((PITBaseFrequency / 100) & 0xFF));
	x86Output(0x80, 0);
	x86Output(0x40, (uint8_t)((PITBaseFrequency / 100) >> 8));
}

void usleep(uint64_t subseconds) {
	sleepclock = subseconds;
	while (sleepclock) {
		asm ("hlt");
	}
}

void sleep(uint64_t seconds) {
	usleep(seconds * 2 * PITHalfSecond);
}

void PITHandler() {
	if (sleepclock) sleepclock--;
	PICSendEndOfInterrupt(0x00);
}
