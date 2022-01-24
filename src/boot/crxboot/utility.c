#include "utility.h"

uint32_t align(uint32_t number, uint32_t alignToHere) {
	if (alignToHere == 0) return number;

	uint32_t remainder = number % alignToHere;
	return (remainder > 0) ? (number + alignToHere - remainder) : number;
}
