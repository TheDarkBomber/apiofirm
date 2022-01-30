#ifndef __SERIAL_KERNEL_H
#define __SERIAL_KERNEL_H

#include <stdbool.h>

bool InitialiseSerial();

int SerialReceived();
int TransmitEmpty();

char SerialRead();
void SerialWrite(char c);

#endif
