[bits 64]

%macro PushAllRegisters 0
  push r15
  push r14
  push r13
  push r12
  push r11
  push r10
  push r9
  push r8
  push rsi
  push rdi
  push rdx
  push rcx
  push rbx
  push rax
%endmacro

%macro PopAllRegisters 0
  pop rax
  pop rbx
  pop rcx
  pop rdx
  pop rdi
  pop rsi
  pop r8
  pop r9
  pop r10
  pop r11
  pop r12
  pop r13
  pop r14
  pop r15
%endmacro

%macro ISRStub 1
ISRStub_%+%1:
	push qword 0
	push qword %1
	push rbp
	mov rbp, rsp
	PushAllRegisters
	mov rdi, rsp
	call InterruptHandler
	PopAllRegisters
	pop rbp
	add rsp, 16
	iretq
%endmacro

extern InterruptHandler
ISRStub 0
ISRStub 1
ISRStub 2
ISRStub 3
ISRStub 4
ISRStub 5
ISRStub 6
ISRStub 7
ISRStub 8
ISRStub 9
ISRStub 10
ISRStub 11
ISRStub 12
ISRStub 13
ISRStub 14
ISRStub 15
ISRStub 16
ISRStub 17
ISRStub 18
ISRStub 19
ISRStub 20
ISRStub 21
ISRStub 22
ISRStub 23
ISRStub 24
ISRStub 25
ISRStub 26
ISRStub 27
ISRStub 28
ISRStub 29
ISRStub 30
ISRStub 31
ISRStub 32
ISRStub 33
ISRStub 34
ISRStub 35
ISRStub 36
ISRStub 37
ISRStub 38
ISRStub 39
ISRStub 40
ISRStub 41
ISRStub 42
ISRStub 43
ISRStub 44
ISRStub 45
ISRStub 46
ISRStub 47

global ISRStubTable
ISRStubTable:
%assign i 0
%rep 48
dq ISRStub_%+i
%assign i i + 1
%endrep
