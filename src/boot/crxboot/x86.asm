%macro x86EnterRealMode 0
	[bits 32]
	jmp word 0x18:.ProtectedModeB16

	.ProtectedModeB16:
	[bits 16]

	mov eax, cr0 									; Disable protected mode flag.
	and al, ~1
	mov cr0, eax

	jmp word 0x00:.RealMode

	.RealMode:
	mov ax, 0 										; Setup segment registers.
	mov ds, ax
	mov ss, ax

	sti 													; Enable BIOS interrupts.
%endmacro

%macro x86EnterProtectedMode 0
	cli 													; Disable BIOS interrupts.

	mov eax, cr0 									; Enable protected mode flag.
	or al, 1
	mov cr0, eax

	jmp dword 0x08:.ProtectedMode

	.ProtectedMode:
	[bits 32]

	mov ax, 0x10 									; Setup segment registers.
	mov ds, ax
	mov ss, ax
%endmacro

;;; CONVERT LINEAR ADDRESS TO SEGMENT OFFSET ADDRESS
;;; %1 = Linear address.
;;; %2 = Output target segment register.
;;; %3 = Target 32-bit register.
;;; %4 = Target lower half of %3.

%macro ConvertLinearAddressToSegmentOffsetAddress 4
	mov %3, %1
	shr %3, 4
	mov %2, %4
	mov %3, %1
	and %3, 0xF
%endmacro

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

;;; bool _cdecl x86DiskReset(uint8_t disk);

global x86DiskReset
x86DiskReset:
	[bits 32]
	push ebp 											; Save old call frame.
	mov ebp, esp 										; Initialise new call frame.

	x86EnterRealMode
	mov ah, 0
	mov dl, [bp + 8] 							; dl stores the disk number.
	stc
	int 0x13

	mov eax, 1
	sbb eax, 0 										; Success = 1.

	push eax
	x86EnterProtectedMode

	pop eax

	mov esp, ebp 										; Antisave old call frame.
	pop ebp
	ret

;;; bool _cdecl x86DiskRead(uint8_t disk, uint16_t cylinder, uint16_t sector, uint16_t head, uint8_t count, void far *outputData);

global x86DiskRead
x86DiskRead:
	push ebp 											; Save old call frame.
	mov ebp, esp 										; Initialise new call frame.

	x86EnterRealMode
	push ebx 											; Save modified registers.
	push es

	mov dl, [bp + 8] 							; Setup arguments.

	mov ch, [bp + 12] 							; dl stores the disk number.
	mov cl, [bp + 13] 							; Lower 8 bits of ch, cylinder
	shl cl, 6

	mov al, [bp + 16] 							; Sector is bits 0-5 of cl.
	and al, 0x3F
	or cl, al

	mov dh, [bp + 20] 						; dh stores the head.

	mov al, [bp + 24] 						; al stores the count.

	ConvertLinearAddressToSegmentOffsetAddress [bp + 28], es, ebx, bx

	mov ah, 0x02 									; Call disk interrupt.
	stc
	int 0x13

	mov eax, 1 										; Set return value.
	sbb eax, 0 										; Success = 1.

	pop es 												; Antisave registers.
	pop ebx

	push eax
	x86EnterProtectedMode

	pop eax

	mov esp, ebp 										; Antisave old call frame.
	pop ebp
	ret

;;; bool _cdecl x86DiskGetParameters(uint8_t disk, uint8_t* outputDriveType, uint16_t* outputCylinders, uint16_t* outputSectors, uint16_t* outputHeads);

global x86DiskGetParameters
x86DiskGetParameters:
	[bits 32]
	push ebp 											; Save old call frame.
	mov ebp, esp 										; Initialise new call frame.

	x86EnterRealMode
	[bits 16]
	push es 											; Save registers.
	push bx
	push si
	push di

	mov dl, [bp + 8] 							; dl stores the disk number.
	mov ah, 0x08
	mov di, 0
	mov es, di
	stc
	int 0x13 											; Call disk interrupt.

	mov ax, 1 										; Return.
	sbb ax, 0

	ConvertLinearAddressToSegmentOffsetAddress [bp + 12], es, esi, si
	mov [es:si], bl

	mov bl, ch
	mov bh, cl
	shr bh, 6
	inc bx

	ConvertLinearAddressToSegmentOffsetAddress [bp + 16], es, esi, si
	mov [es:si], bx

	xor ch, ch
	and cl, 0x3F

	ConvertLinearAddressToSegmentOffsetAddress [bp + 20], es, esi, si
	mov [es:si], cx

	mov cl, dh
	inc cx

	ConvertLinearAddressToSegmentOffsetAddress [bp + 24], es, esi, si
	mov [es:si], cx

	pop di 												; Antisave registers.
	pop si
	pop bx
	pop es

	push eax
	x86EnterProtectedMode
	[bits 32]
	pop eax

	mov esp, ebp 										; Antisave old call frame.
	pop ebp
	ret

