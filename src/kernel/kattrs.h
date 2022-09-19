#ifndef __APIOFIRM_KATTRS_H_
#define __APIOFIRM_KATTRS_H_
#include <stdint.h>

typedef struct {
	uint64_t RSVP : 63;
	uint64_t EOI : 1;
} KernelAttributes;

void ApiofirmEnableEOI();
void ApiofirmDisableEOI();

extern KernelAttributes ApiofirmCTX;

#endif
