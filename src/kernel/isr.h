#ifndef __ISR_ISR_KERNEL_H
#define __ISR_ISR_KERNEL_H
#include <stdint.h>

#define EXCEPTION_DIVIDE_BY_ZERO 0x00
#define EXCEPTION_DEBUG 0x01
#define EXCEPTION_NON_MASKABLE 0x02
#define EXCEPTION_BREAKPOINT 0x03
#define EXCEPTION_OVERFLOW 0x04
#define EXCEPTION_BOUND_RANGE_EXCEEDED 0x05
#define EXCEPTION_INVALID_OPCODE 0x06
#define EXCEPTION_DEVICE_NOT_AVAILABLE 0x07
#define EXCEPTION_DOUBLE_FAULT 0x08
#define EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN 0x09
#define EXCEPTION_INVALID_TSS 0x0A
#define EXCEPTION_SEGMENT_NOT_PRESENT 0x0B
#define EXCEPTION_STACK_SEGMENT 0x0C
#define EXCEPTION_GENERAL_PROTECTION 0x0D
#define EXCEPTION_PAGE 0x0E
#define EXCEPTION_FLOATING_POINT 0x10
#define EXCEPTION_ALIGNMENT_CHECK 0x11
#define EXCEPTION_MACHINE_CHECK 0x12
#define EXCEPTION_SIMD_FLOATING_POINT 0x13
#define EXCEPTION_VIRTUALISAION 0x14
#define EXCEPTION_CONTROL_PROTECTION 0x15
#define EXCEPTION_HYPERVISOR_INJECTION 0x1C
#define EXCEPTION_VMM_COMMUNICATION 0x1D
#define EXCEPTION_SECURITY 0x1E
#define EXCEPTION_FPU_ERROR 0x2D

void KernelPanic(const char* message, uint8_t exception);

#endif