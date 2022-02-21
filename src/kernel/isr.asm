%macro ISRErrorStub 1
ISRStub_%+%1:
	push dword %1
	call exceptionHandler
	pop eax
	iret
%endmacro

%macro ISRNormalStub 1
ISRStub_%+%1:
	push dword %1
	call interruptHandler
	pop eax
	iret
%endmacro

%macro ISR0x20Stub 1
ISRStub_%+%1:
	push dword %1
	call timeHandler
	pop eax
	iret
%endmacro

%macro ISR0x21Stub 1
ISRStub_%+%1:
	push dword %1
	call keyboardHandler
	pop eax
	iret
%endmacro

%macro ISR0x26Stub 1
ISRStub_%+%1:
	push dword %1
	call floppyHandler
	pop eax
	iret
%endmacro

extern exceptionHandler
extern interruptHandler
extern timeHandler
extern keyboardHandler
extern floppyHandler

ISRErrorStub  0
ISRErrorStub  1
ISRErrorStub  2
ISRErrorStub  3
ISRErrorStub  4
ISRErrorStub  5
ISRErrorStub  6
ISRErrorStub  7
ISRErrorStub  8
ISRErrorStub  9
ISRErrorStub  10
ISRErrorStub  11
ISRErrorStub  12
ISRErrorStub  13
ISRErrorStub  14
ISRErrorStub  15
ISRErrorStub  16
ISRErrorStub  17
ISRErrorStub  18
ISRErrorStub  19
ISRErrorStub  20
ISRErrorStub  21
ISRErrorStub  22
ISRErrorStub  23
ISRErrorStub  24
ISRErrorStub  25
ISRErrorStub  26
ISRErrorStub  27
ISRErrorStub  28
ISRErrorStub  29
ISRErrorStub  30
ISRErrorStub  31
ISR0x20Stub   32
ISR0x21Stub   33
ISRNormalStub 34
ISRNormalStub 35
ISRNormalStub 36
ISRNormalStub 37
ISR0x26Stub   38
ISRNormalStub 39
ISRNormalStub 40
ISRNormalStub 41
ISRNormalStub 42
ISRNormalStub 43
ISRNormalStub 44
ISRErrorStub  45
ISRNormalStub 46
ISRNormalStub 47

global ISRStubTable
ISRStubTable:
%assign i 0
%rep 48
	dd ISRStub_%+i
%assign i i + 1
%endrep
