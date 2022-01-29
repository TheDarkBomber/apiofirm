#ifndef __SERIAL_KERNEL_H
#define __SERIAL_KERNEL_H

#include <stdbool.h>

static bool InitialiseSerial();

int SerialReceived();
int TransmitEmpty();

char SerialRead();
void SerialWrite(char c);

#endif
