#ifndef __APIOFIRM_SERIAL_H_
#define __APIOFIRM_SERIAL_H_
#include <stdbool.h>

#define COM1Port 0x3F8

bool InitialiseSerial();
unsigned SerialTransmissionEmpty();
void SerialWrite(char c);
void SerialWriteString(char* s);

#endif
