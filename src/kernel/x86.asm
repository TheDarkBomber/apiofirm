global x86Input
x86Input:
	[bits 32]
	mov dx, [esp + 4]
	xor eax, eax
	in al, dx
	ret

global x86Output
x86Output:
	[bits 32]
	mov dx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	ret

global x86ReadCR0
x86ReadCR0:
	[bits 32]
	mov eax, cr0
	ret

global x86WriteCR0
x86WriteCR0:
	[bits 32]
	push ebp
	mov ebp, esp
	mov eax, [ebp + 8]
	mov cr0, eax
	pop ebp
	ret

global x86ReadCR2
x86ReadCR2:
	[bits 32]
	mov eax, cr2
	ret

global x86WriteCR2
x86WriteCR2:
	[bits 32]
	push ebp
	mov ebp, esp
	mov eax, [ebp + 8]
	mov cr2, eax
	pop ebp
	ret

global x86ReadCR3
x86ReadCR3:
	[bits 32]
	mov eax, cr3
	ret

global x86WriteCR3
x86WriteCR3:
	[bits 32]
	push ebp
	mov ebp, esp
	mov eax, [ebp + 8]
	mov cr3, eax
	pop ebp
	ret

global x86ReadEFLAGS
x86ReadEFLAGS:
	[bits 32]
	pushf
	pop eax
	ret
