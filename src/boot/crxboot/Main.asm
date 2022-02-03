bits 16

section .entry
extern __bss_start
extern __end

extern cstart_
global entry

entry:
	mov [BootDisk], dl 						; dl contains the boot disk number.

	call CreateMemoryMap 					; Create a memory map for the kernel to use.
	cli

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

CreateMemoryMap:
	[bits 16]
	mov di, 0x8004 								; Origin of memory map.
	xor ebx, ebx 									; Clear ebx.
	xor bp, bp 										; Entry count stored in bp.
	mov edx, 0x0534D4150 					; Magical number 'SMAP'.
	mov eax, 0xE820 							; For interrupt 0x15.
	mov [es:di + 20], dword 1 		; ACPI 3.0.
	mov ecx, 24 									; Each memory map region is 24 bytes in length.
	int 0x15 											; Memory map interrupt, uses value set in eax.
	jc short .failure 						; Unsupported function.
	mov edx, 0x0534D4150 					; Reset magical number.
	cmp eax, edx
	jne short .failure
	test ebx, ebx 								; Test if list is 1 entry in length.
	je short .failure
	jmp short .next
.loop:
	mov eax, 0xE820 							; Reset value to 0xE820.
	mov [es:di + 20], dword 1 		; ACPI 3.0.
	mov ecx, 24 									; Each memory map region is 24 bytes in length.
	int 0x15 											; Memory map interrupt.
	jc short .finished 						; If the carry flag is set, the the memory map is whole.
	mov edx, 0x0534D4150 					; Reset magical number.
.next:
	jcxz .skipentry 							; Skip entries with no length.
	cmp cl, 20 										; Test if interrupt 0x15 response is valid.
	jbe short .notext
	test byte [es:di + 20], 1 		; Test if data should be ignored.
	je short .skipentry
.notext:
	mov ecx, [es:di + 8] 					; Lower 32 bits of memory region length.
	or ecx, [es:di + 12] 					; Bitwise or with upper 32 bits.
	jz .skipentry 								; If it happens to be 0, skip the entry.
	inc bp 												; Increment entry count.
	add di, 24
.skipentry:
	test ebx, ebx 								; Indicates whole memory map if 0.
	jne short .loop
.finished:
	mov [MemoryMapEntries], bp 		; Store the entry count at known location in memory.
	clc 													; Clear the carry flag.
	ret
.failure:
	stc 													; Set the carry flag if the function is unsupported.
	ret

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

MemoryMapEntries equ 0x8000 		; Stores memory map entry count at 0x8000.

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

