[bits 64]
global LoadGDT
section .text
LoadGDT:
	lgdt [rdi]
	mov rax, 0x08
	push rax
	push .reload
	retfq
	.reload:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
