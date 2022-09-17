#ifndef __APIOFIRM_PIT_H_
#define __APIOFIRM_PIT_H_
#include <stdint.h>

#define PITBaseFrequency 1193182
#define PITHalfSecond 11932

void sleep(uint64_t seconds);
void usleep(uint64_t subseconds);

void InitialisePIT();
void PITHandler();

#endif
