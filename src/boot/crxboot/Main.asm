bits 16

section .entry
extern __bss_start
extern __end

extern cstart_
global entry

entry:
	cli

	mov [BootDisk], dl 						; dl contains the boot disk number.

	mov ax, ds 										; Setup the stack.
	mov ss, ax
	mov sp, 0xFFF0
	mov bp, sp

	call EnableA20 								; Enable A20 gate.
	call LoadGDT 									; Load the Global Descriptor Table.

	mov eax, cr0 									; Enable protected mode flag.
	or al, 1
	mov cr0, eax

	jmp dword 0x08:.ProtectedMode ; Welcome to protected mode.

	.ProtectedMode:
	[bits 32]

	mov ax, 0x10 									; Setup segment registers.
	mov ds, ax
	mov ss, ax

	mov edi, __bss_start 					; Destroy uninitialised data.
	mov ecx, __end
	sub ecx, edi
	mov al, 0
	cld
	rep stosb

	xor edx, edx
	mov dl, [BootDisk] 						; Put BootDisk back into dl so that cstart_ can use it.
	push edx
	call cstart_

	cli
	hlt

EnableA20:
	[bits 16]
	call A20WaitInput 						; Disable keyboard.
	mov al, DisableKeyboard
	out KeyboardCommandPort, al

	call A20WaitInput 						; Read the control output port.
	mov al, KeyboardRControlOutputPort
	out KeyboardCommandPort, al

	call A20WaitOutput
	in al, KeyboardDataPort
	push eax

	call A20WaitInput 						; Write to the control output port.
	mov al, KeyboardWControlOutputPort
	out KeyboardCommandPort, al

	call A20WaitInput
	pop eax
	or al, 2 											; A20.
	out KeyboardDataPort, al

	call A20WaitInput 						; Enable keyboard.
	mov al, EnableKeyboard
	out KeyboardCommandPort, al

	call A20WaitInput
	ret

;;; Waits until status bit 2 is 0.
A20WaitInput:
	[bits 16]
	in al, KeyboardCommandPort
	test al, 2
	jnz A20WaitInput
	ret

;;;  Wiats until status bit 1 is 1.
A20WaitOutput:
	[bits 16]
	in al, KeyboardCommandPort
	test al, 1
	jz A20WaitOutput
	ret

LoadGDT:
	[bits 16]
	lgdt [MetaGDT]
	ret

KeyboardDataPort equ 0x60
KeyboardCommandPort equ 0x64

DisableKeyboard equ 0xAD
EnableKeyboard equ 0xAE

KeyboardRControlOutputPort equ 0xD0
KeyboardWControlOutputPort equ 0xD1

ScreenBuffer equ 0xB8000

GDT:
	dq 0 													; NULL

	dw 0xFFFF 										; Limit = 0xFFFF
	dw 0 													; Base, bits 0 to 15.
	db 0 													; Base, bits 16 to 23.
	db 0b10011010 								; Access {Present, Ring=0, Code, Executable, Direction=0, Read}
	db 0b11001111 								; Granularity {Pages=4kB,Mode=Protected32}
	db 0

	dw 0xFFFF
	dw 0
	db 0
	db 0b10010010 								; Access {Present, Ring=0, Data, Executable, Direction=0, Write}
	db 0b11001111 								; Granularity {Pages=4kB,Mode=Protected32}
	db 0

	dw 0xFFFF
	dw 0
	db 0
	db 0b10011010 								; Access {Present, Ring=0, Code, Executable, Direction=0, Read}
	db 0b00001111 								; Granularity {Pages=1B,Mode=Protected16}
	db 0

	dw 0xFFFF
	dw 0
	db 0
	db 0b10010010 								; Access {Present, Ring=0, Data, Executable, Direction=0, Write}
	db 0b00001111 								; Granularity {Pages=1B,Mode=Protected16}
	db 0

MetaGDT:
	dw MetaGDT - GDT - 1 					; Size of GDT.
	dd GDT 												; Address of GDT.

BootDisk:	db 0

