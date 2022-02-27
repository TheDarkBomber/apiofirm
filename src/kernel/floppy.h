#ifndef __FLOPPY_DISK_CONTROLLER_H
#define __FLOPPY_DISK_CONTROLLER_H
#include <stdint.h>
#include <stdbool.h>

enum FloppyRegisters {
	FLOPPY_STATUS_A = 0x3F0,
	FLOPPY_STATUS_B = 0x3F1,
	FLOPPY_DIGITAL_OUTPUT = 0x3F2,
	FLOPPY_TAPE_DRIVE = 0x3F3,
	FLOPPY_PRIMARY_STATUS = 0x3F4,
	FLOPPY_DATARATE_SELECT = 0x3F4,
	FLOPPY_DATA_QUEUE = 0x3F5,
	FLOPPY_INPUT = 0x3F7,
	FLOPPY_CONFIGURATION = 0x3F7
};

enum FloppyCommands {
	FLOPPY_READ_TRACK = 2,
	FLOPPY_SPECIFY = 3,
	FLOPPY_DRIVE_STATUS = 4,
	FLOPPY_WRITE = 5,
	FLOPPY_READ = 6,
	FLOPPY_RECALIBRATE = 7,
	FLOPPY_INTERRUPT = 8,
	FLOPPY_WRITE_DELETED = 9,
	FLOPPY_READ_ID = 10,
	FLOPPY_READ_DELETED = 12,
	FLOPPY_FORMAT_TRACK = 13,
	FLOPPY_DUMP_REGISTERS = 14,
	FLOPPY_SEEK = 15,
	FLOPPY_VERSION = 16,
	FLOPPY_SCAN_EQUAL = 17,
	FLOPPY_PERPENDICULAR = 18,
	FLOPPY_CONFIGURE = 19,
	FLOPPY_LOCK = 20,
	FLOPPY_VERIFY = 22,
	FLOPPY_SCAN_LOW_OR_EQUAL = 25,
	FLOPPY_SCAN_HIGH_OR_EQUAl = 29,
	FLOPPY_SKIP = 0x20,
	FLOPPY_DENSITY = 0x40,
	FLOPPY_MULTITRACK = 0x80,
	FLOPPY_MDS = FLOPPY_MULTITRACK | FLOPPY_DENSITY | FLOPPY_SKIP
};

typedef enum {
	FLOPPY_MOTOR_INACTIVE = 0,
	FLOPPY_MOTOR_ACTIVE = 1,
	FLOPPY_MOTOR_WAITING = 2
} FloppyMotorStates;

void FloppyDetectDrives();
char* FloppyDetectDrive();
void FloppyCheckInterrupt(uint8_t* ST0, uint8_t* cylinder);
void FloppyWrite(uint8_t command);
uint8_t FloppyRead();
void ResetFloppy();
void FloppyMotor(bool state);
void DestroyFloppyMotor();

uint16_t FloppyCalibrate();
bool FloppySeek(uint8_t cylinder, uint8_t head);

static void InitialiseFloppyDMA(bool write);
bool FloppyTrack(uint8_t cylinder, uint8_t head, uint8_t sector, bool write);
#endif
