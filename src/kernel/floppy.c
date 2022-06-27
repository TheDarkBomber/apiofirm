#include "floppy.h"
#include "task.h"
#include "isr.h"
#include "memory.h"
#include "pic.h"
#include "pit.h"
#include "x86.h"
#include "comstdio.h"
#include <stdint.h>
#include <stdbool.h>

#define FLOPPY_DMA_LENGTH 0x4800

static Task floppyTimerTask;
static volatile bool FloppyAchknowledgeIRQ = false;

static volatile FloppyMotorStates FloppyMotorState = FLOPPY_MOTOR_INACTIVE;
static volatile uint16_t FloppyMotorTime = 0;

const char FloppyDMABuffer[FLOPPY_DMA_LENGTH];

static const char *FloppyDriveTypes[8] = {
	"UNKNOWN",
	"360K",
	"1.44M",
	"720K",
	"1.2M",
	"2.88M",
	"UNKNOWN",
	"UNKNOWN"
};

void FloppyDetectDrives() {
	x86Output(0x70, FLOPPY_READ_ID);
	uint8_t drives = x86Input(0x71);

	cprint("[FLOPPY] Drive 0: %s\r\n", FloppyDriveTypes[drives >> 4]);
	cprint("[FLOPPY] Drive 1: %s\r\n", FloppyDriveTypes[drives & 0x0F]);
}

char* FloppyDetectDrive() {
	x86Output(0x70, FLOPPY_READ_ID);
	return (char*)FloppyDriveTypes[x86Input(0x71) >> 4];
}

void floppyHandler(uint8_t interrupt) {
	FloppyAchknowledgeIRQ = true;
	PICSendEndOfInterrupt(interrupt);
}

void FloppyWrite(uint8_t command) {
	for (uint16_t i = 0; i < 6 * 182; i++) {
		Sleep(1);
		if (0x80 & x86Input(FLOPPY_PRIMARY_STATUS)) {
			x86Output(FLOPPY_DATA_QUEUE, command);
			return;
		}
	}
	KernelPanic("Failure to write command to floppy disk controller.", 0xC8);
}

uint8_t FloppyRead() {
	for (uint16_t i = 0; i < 600; i++) {
		Sleep(1);
		if (0x80 & x86Input(FLOPPY_PRIMARY_STATUS)) {
			return x86Input(FLOPPY_DATA_QUEUE);
		}
	}
	KernelPanic("Failure to read data from floppy disk controller.", 0xC8);
	return 0;
}

void ResetFloppy() {
	x86Output(FLOPPY_DIGITAL_OUTPUT, 0x00);
	x86Output(FLOPPY_DIGITAL_OUTPUT, 0x0C);
	while (!FloppyAchknowledgeIRQ);
	FloppyAchknowledgeIRQ = false;
	uint8_t ST0, cylinder;
	for (uint8_t i = 0; i < 4; i++) FloppyCheckInterrupt(&ST0, &cylinder);
	x86Output(FLOPPY_CONFIGURATION, 0x00);
	FloppyWrite(FLOPPY_SPECIFY);
	FloppyWrite(0xDF);
	FloppyWrite(0x02);

	if (FloppyCalibrate()) return KernelPanic("Floppy calibration failure.", 0xC8);
}

uint16_t FloppyCalibrate() {
	FloppyMotor(true);
	uint8_t ST0, cylinder;
	for (uint32_t i = 0; i < 10; i++) {
		FloppyWrite(FLOPPY_RECALIBRATE);
		FloppyWrite(0);

		while (!FloppyAchknowledgeIRQ);
		FloppyAchknowledgeIRQ = false;
		FloppyCheckInterrupt(&ST0, &cylinder);
		if (ST0 & 0xC0) {
			static const char* status[4] = { 0, "ERROR", "INVALID", "DRIVE" };
			if ((ST0 >> 6) != 0) cprint("[FLOPPY] Calibration error: %s\r\n", status[ST0 >> 6]);
			continue;
		}

		if (!cylinder) {
			FloppyMotor(false);
			return 0;
		}
	}

	FloppyMotor(false);
	cprint("[FLOPPY] Retry attempts at calibration exhausted.\r\n");
	return 1;
}

void FloppyCheckInterrupt(uint8_t *ST0, uint8_t *cylinder) {
	FloppyWrite(FLOPPY_INTERRUPT);
	*ST0 = FloppyRead();
	*cylinder = FloppyRead();
}

void floppyTimer() {
	for (;;) {
		Sleep(1);
		if (FloppyMotorState == FLOPPY_MOTOR_WAITING) {
			FloppyMotorTime -= 50;
			if (FloppyMotorTime == 0) {
				DestroyFloppyMotor();
				break;
			}
		}
		YieldTask();
	}
	DestroyTask(&floppyTimerTask);
	END_TASK;
}

void FloppyMotor(bool state) {
	if (state) {
		if (!FloppyMotorState) {
			x86Output(FLOPPY_DIGITAL_OUTPUT, 0x1C);
			Sleep(3);
		} else {
			if (FloppyMotorState == FLOPPY_MOTOR_WAITING) cprint("[FLOPPY] Waiting...\r\n");
			FloppyMotorTime = 300;
			FloppyMotorState = FLOPPY_MOTOR_WAITING;
			CreateTask(&floppyTimerTask, floppyTimer);
			AppendTask(&floppyTimerTask);
		}
	}
}

void DestroyFloppyMotor() {
	x86Output(FLOPPY_DIGITAL_OUTPUT, 0x0C);
	FloppyMotorState = FLOPPY_MOTOR_INACTIVE;
}

bool FloppySeek(uint8_t cylinder, uint8_t head) {
	uint8_t ST0, cylinderCheck;
	FloppyMotor(true);
	for (uint8_t i = 0; i < 10; i++) {
		FloppyWrite(FLOPPY_SEEK);
		FloppyWrite(head << 2);
		FloppyWrite(cylinder);

		while (!FloppyAchknowledgeIRQ);
		FloppyAchknowledgeIRQ = false;
		FloppyCheckInterrupt(&ST0, &cylinderCheck);

		if (ST0 & 0xC0) {
			static const char* status[4] = {0, "error", "invalid", "drive"};
			if ((ST0 >> 6) != 0) cprint("[FLOPPY] Seek status: %s\r\n", status[ST0 >> 6]);
			continue;
		}

		if (cylinder == cylinderCheck) {
			FloppyMotor(false);
			return true;
		}
	}

	FloppyMotor(false);
	cprint("[FLOPPY] Seek error, attempts exhausted.\r\n");
	return false;
}

static void InitialiseFloppyDMA(bool write) {
	union {
		uint8_t Byte[4];
		uint32_t Long;
	} address, count;

	address.Long = (uint32_t) &FloppyDMABuffer;
	count.Long = (uint32_t) FLOPPY_DMA_LENGTH - 1;

	if (!write) memset(FloppyDMABuffer, 0, FLOPPY_DMA_LENGTH);

	x86Output(0x0A, 0x06);
	x86Output(0x0C, 0xFF);
	x86Output(0x04, address.Byte[0]);
	x86Output(0x04, address.Byte[1]);
	x86Output(0x81, address.Byte[2]);
	x86Output(0x0C, 0xFF);
	x86Output(0x05, count.Byte[0]);
	x86Output(0x05, count.Byte[1]);
	x86Output(0x80, 0);
	x86Output(0x0A, 0x02);
	x86Output(0x0A, 0x06);
	x86Output(0x0B, write ? 0x5A : 0x56);
	x86Output(0x0A, 0x02);
}

bool FloppyTrack(uint8_t cylinder, uint8_t head, uint8_t sector, bool write) {
	uint8_t command;
	static const uint8_t flags = 0xC0;
	if (write) command = FLOPPY_WRITE | FLOPPY_MDS;
	else command = FLOPPY_READ | FLOPPY_MDS;

	if (!FloppySeek(cylinder, 0)) return false;
	if (!FloppySeek(cylinder, 1)) return false;

	for (uint8_t i = 0; i < 20; i++) {
		FloppyMotor(true);
		InitialiseFloppyDMA(write);
		Sleep(4);
		FloppyWrite(command);
		FloppyWrite(head << 2);
		FloppyWrite(cylinder);
		FloppyWrite(head);
		FloppyWrite(sector);
		FloppyWrite(2);
		FloppyWrite(sector >= 18 ? 18 : sector);
		FloppyWrite(0x1B);
		FloppyWrite(0xFF);
		while (!FloppyAchknowledgeIRQ);
		FloppyAchknowledgeIRQ = false;
		uint8_t ST0, ST1, ST2, outputCylinder, outputHead, outputSector, bytesPSector;
		ST0 = FloppyRead();
		ST1 = FloppyRead();
		ST2 = FloppyRead();

		outputCylinder = FloppyRead();
		outputHead = FloppyRead();
		outputSector = FloppyRead();

		bytesPSector = FloppyRead();

		bool error = false;

		if (ST0 & 0xC0) {
			static const char* status[4] = {0, "ERROR", "INVALID COMMAND", "DRIVE NOT READY" };
			if ((ST0 >> 6) != 0) cprint("[FLOPPY] Track error status: %s\r\n", status[ST0 >> 6]);
			error = true;
		}

		if (ST1 & 0x80) {
			cprint("[FLOPPY] Track error, end of cylinder.\r\n");
			error = true;
		}

		if (ST0 & 0x80) {
			cprint("[FLOPPY] Track error, drive not ready.\r\n");
			error = true;
		}

		if (ST1 & 0x20) {
			cprint("[FLOPPY] Track error, cyclic redundancy check made redundant.\r\n");
			error = true;
		}

		if (ST1 & 0x10) {
			cprint("[FLOPPY] Track error, floppy disk controller timeout.\r\n");
			error = true;
		}

		if (ST1 & 0x04) {
			cprint("[FLOPPY] Track error, no data found.\r\n");
			error = true;
		}

		if ((ST1 | ST2) & 0x01) {
			cprint("[FLOPPY] Track error, no address mark.\r\n");
			error = true;
		}

		if (ST2 & 0x40) {
			cprint("[FLOPPY] Track error, deleted address.\r\n");
			error = true;
		}

		if (ST2 & 0x20) {
			cprint("[FLOPPY] Track error, cyclic redundancy check on fire.\r\n");
			error = true;
		}

		if (ST2 & 0x10) {
			cprint("[FLOPPY] Track error, you have the wrong cylinder.\r\n");
			error = true;
		}

		if (ST2 & 0x04) {
			cprint("[FLOPPY] Track error, 404 UPD765 not found.\r\n");
			error = true;
		}

		if (ST2 & 0x02) {
			cprint("[FLOPPY] Track error, cylinder skill issue.\r\n");
			error = true;
		}

		if (bytesPSector != 0x02) {
			cprint("[FLOPPY] Track error, I wanted 512 bytes per sector. And yet you gave me %u bytes per sector.\r\n", (1 << (bytesPSector + 7)));
			error = true;
		}

		if (ST1 & 0x02) {
			cprint("[FLOPPY] Track error, this disk cannot be written to.\r\n");
			error = true;
		}

		FloppyMotor(false);
		if (!error) return true;
		return false;
	}

	cprint("[FLOPPY] Track error, attempts exhausted.\r\n");
	FloppyMotor(false);
	return false;
}
