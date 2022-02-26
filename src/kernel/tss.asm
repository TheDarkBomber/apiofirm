global FlushTSS
FlushTSS:
	mov ax, 0x28
	ltr ax
	ret


extern metaGDT
global FlushGDT
FlushGDT:
	lgdt [metaGDT]
	mov eax, 0x08
	push eax
	push .reload
	retf
	.reload:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
