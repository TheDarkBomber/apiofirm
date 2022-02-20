#ifndef __PIT_TIME_KERNEL_H
#define __PIT_TIME_KERNEL_H
#include <stdint.h>

#define PIT_FREQUENCY 1193182
void SetPITFrequency(uint16_t frequency);
void Sleep(uint32_t delay);

#endif
