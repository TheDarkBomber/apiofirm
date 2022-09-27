#include "x86.h"

void x86Output(uint16_t port, uint8_t value);
uint8_t x86Input(uint16_t port);
void x86OutputWide(uint16_t port, uint16_t value);
uint16_t x86InputWide(uint16_t port);
void x86OutputDouble(uint16_t port, uint32_t value);
uint32_t x86InputDouble(uint16_t port);
