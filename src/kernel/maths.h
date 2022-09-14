#ifndef __APIOFIRM_MATHS_UTILITIES_
#define __APIOFIRM_MATHS_UTILITIES_
#include <stdint.h>

inline uint64_t U64CeilingDivision(uint64_t numerator, uint64_t denominator) {
	if (numerator % denominator == 0) return numerator / denominator;
	else return (numerator / denominator) + 1;
}

#endif
