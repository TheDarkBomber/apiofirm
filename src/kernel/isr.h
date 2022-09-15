#ifndef __APIOFIRM_ISR_H_
#define __APIOFIRM_ISR_H_

typedef enum {
	ISR_DivideByZero = 0x00,
	ISR_Debug = 0x01,
	ISR_NMI = 0x02,
	ISR_Breakpoint = 0x03,
	ISR_Overflow = 0x04,
	ISR_BoundRangeExceeded = 0x05,
	ISR_InvalidOpcode = 0x06,
	ISR_DNA = 0x07,
	ISR_DoubleFault = 0x08,
	ISR_CoprocessorSegmentOverrun = 0x09,
	ISR_InvalidTSS = 0x0A,
	ISR_SegmentNotPresent = 0x0B,
	ISR_StackSegment = 0x0C,
	ISR_GeneralProtection = 0x0D,
	ISR_Page = 0x0E,
	ISR_x87 = 0x10,
	ISR_AlignmentCheck = 0x11,
	ISR_MachineCheck = 0x12,
	ISR_SIMD = 0x13,
	ISR_Virtualisation = 0x14,
	ISR_ControlProtection = 0x15,
	ISR_HypervisorInjection = 0x1C,
	ISR_VMMCommunication = 0x1D,
	ISR_Security = 0x1E,
	ISR_Timer = 0x20,
	ISR_Keyboard = 0x21,
	ISR_Cascade = 0x22,
	ISR_COM2 = 0x23,
	ISR_COM1 = 0x24,
	ISR_LPT2 = 0x25,
	ISR_Floppy = 0x26,
	ISR_LPT1 = 0x27,
	ISR_CMOS = 0x28,
	ISR_Mouse = 0x29,
	ISR_FPU = 0x2A,
	ISR_PrimaryATA = 0x2E,
	ISR_SecondaryATA = 0x2F
} InterruptServiceRoutine;

extern char* ISR_Strings[];

#endif
