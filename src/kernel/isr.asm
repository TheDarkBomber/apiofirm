%macro ISRErrorStub 1
ISRStub_%+%1:
	call exceptionHandler
	iret
%endmacro

%macro ISRNormalStub 1
ISRStub_%+%1:
	push dword %1
	call interruptHandler
	pop eax
	iret
%endmacro

extern exceptionHandler
extern interruptHandler

ISRNormalStub 0
ISRNormalStub 1
ISRNormalStub 2
ISRNormalStub 3
ISRNormalStub 4
ISRNormalStub 5
ISRNormalStub 6
ISRNormalStub 7
ISRErrorStub  8
ISRNormalStub 9
ISRErrorStub  10
ISRErrorStub  11
ISRErrorStub  12
ISRErrorStub  13
ISRErrorStub  14
ISRNormalStub 15
ISRNormalStub 16
ISRErrorStub  17
ISRNormalStub 18
ISRNormalStub 19
ISRNormalStub 20
ISRNormalStub 21
ISRNormalStub 22
ISRNormalStub 23
ISRNormalStub 24
ISRNormalStub 25
ISRNormalStub 26
ISRNormalStub 27
ISRNormalStub 28
ISRNormalStub 29
ISRErrorStub  30
ISRNormalStub 31
ISRNormalStub 32
ISRNormalStub 33
ISRNormalStub 34
ISRNormalStub 35
ISRNormalStub 36
ISRNormalStub 37
ISRNormalStub 38
ISRNormalStub 39
ISRNormalStub 40
ISRNormalStub 41
ISRNormalStub 42
ISRNormalStub 43
ISRNormalStub 44
ISRNormalStub 45
ISRNormalStub 46
ISRNormalStub 47

global ISRStubTable
ISRStubTable:
%assign i 0
%rep 48
	dd ISRStub_%+i
%assign i i + 1
%endrep
