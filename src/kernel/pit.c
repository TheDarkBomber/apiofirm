#include "pit.h"
#include "pic.h"
#include "task.h"
#include "x86.h"
#include <stdint.h>
#include <stdbool.h>

static bool sleeping = false;
static uint32_t timer = 0;
static uint8_t taskTime = 0;

void timeHandler(uint8_t interrupt) {
	if (sleeping) ++timer;
	PICSendEndOfInterrupt(interrupt);
	if (taskTime > 0x09) {
		taskTime = 0;
		YieldTask();
	} else ++taskTime;
}

void SetPITFrequency(uint16_t frequency) {
	uint16_t divisor = PIT_FREQUENCY / frequency;
	x86Output(0x43, 0x36);
	x86Output(0x40, divisor & 0xFF);
	x86Output(0x40, divisor >> 8);
}

void Sleep(uint32_t delay) {
	sleeping = true;
	while (timer < delay);
	sleeping = false;
	timer = 0;
}
