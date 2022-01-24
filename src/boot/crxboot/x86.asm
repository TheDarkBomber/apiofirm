bits 16

section _TEXT class=CODE

;;; void _cdecl x86Divide64By32(uint64_t numerator, uint32_t denominator, uint64_t* outputQuotient, uint32_t* outputRemainder);

global _x86Divide64By32
_x86Divide64By32:
	push bp 											; Save old call frame.
	mov bp, sp 										; Initialise new call frame.

	push bx

	mov eax, [bp + 8] 						; eax = upper 32 bits of numerator.
	mov ecx, [bp + 12] 						; ecx = denominator.
	xor edx, edx
	div ecx 											; eax - quotient, edx - remainder

	mov bx, [bp + 16] 						; Store upper 32 bits of quotient.
	mov [bx + 4], eax

	mov eax, [bp + 4] 						; eax = lower 32 bits of numerator.
	div ecx 											; edx = old remainder.

	mov [bx], eax 								; Store the result.
	mov bx, [bp + 18]
	mov [bx], edx

	pop bx

	mov sp, bp 										; Antisave old call frame.
	pop bp
	ret

;;; void _cdecl x86TeletypeModeWriteCharacter(char c, uint8_t page);

global _x86TeletypeModeWriteCharacter
_x86TeletypeModeWriteCharacter:
	push bp 											; Save old call frame.
	mov bp, sp 										; Initialise new call frame.

	push bx 											; Save bx.

	mov ah, 0x0E 									; Set TTY mode.
	mov al, [bp + 4] 							; Set character.
	mov bh, [bp + 6] 							; Set page.

	int 0x10

	pop bx 												; Antisave bx.

	mov sp, bp 										; Antisave old call frame.
	pop bp
	ret

;;; bool _cdecl x86DiskReset(uint8_t disk);

global _x86DiskReset
_x86DiskReset:
	push bp 											; Save old call frame.
	mov bp, sp 										; Initialise new call frame.

	mov ah, 0
	mov dl, [bp + 4] 							; dl stores the disk number.
	stc
	int 0x13

	mov ax, 1
	sbb ax, 0 										; Success = 1.

	mov sp, bp 										; Antisave old call frame.
	pop bp
	ret

;;; bool _cdecl x86DiskRead(uint8_t disk, uint16_t cylinder, uint16_t sector, uint16_t head, uint8_t count, void far *outputData);

global _x86DiskRead
_x86DiskRead:
	push bp 											; Save old call frame.
	mov bp, sp 										; Initialise new call frame.

	push bx 											; Save modified registers.
	push es

	mov dl, [bp + 4] 							; Setup arguments.

	mov ch, [bp + 6] 							; dl stores the disk number.
	mov cl, [bp + 7] 							; Lower 8 bits of ch, cylinder
	shl cl, 6

	mov al, [bp + 8] 							; Sector is bits 0-5 of cl.
	and al, 0x3F
	or cl, al

	mov dh, [bp + 10] 						; dh stores the head.

	mov al, [bp + 12] 						; al stores the count.

	mov bx, [bp + 16] 						; es:bx is a far pointer to outputData.
	mov es, bx
	mov bx, [bp + 14]

	mov ah, 0x02 									; Call disk interrupt.
	stc
	int 0x13

	mov ax, 1 										; Set return value.
	sbb ax, 0 										; Success = 1.

	pop es 												; Antisave registers.
	pop bx

	mov sp, bp 										; Antisave old call frame.
	pop bp
	ret

;;; bool _cdecl x86DiskGetParameters(uint8_t disk, uint8_t* outputDriveType, uint16_t* outputCylinders, uint16_t* outputSectors, uint16_t* outputHeads);

global _x86DiskGetParameters
_x86DiskGetParameters:
	push bp 											; Save old call frame.
	mov bp, sp 										; Initialise new call frame.

	push es 											; Save registers.
	push bx
	push si
	push di

	mov dl, [bp + 4] 							; dl stores the disk number.
	mov ah, 0x08
	mov di, 0
	mov es, di
	stc
	int 0x13 											; Call disk interrupt.

	mov ax, 1 										; Return.
	sbb ax, 0

	mov si, [bp + 6] 							; Set output parameters.
	mov [si], bl

	mov bl, ch
	mov bh, cl
	shr bh, 6
	mov si, [bp + 8]
	mov [si], bx

	xor ch, ch
	and cl, 0x3F
	mov si, [bp + 10]
	mov [si], cx

	mov cl, dh
	mov si, [bp + 12]
	mov [si], cx

	pop di 												; Antisave registers.
	pop si
	pop bx
	pop es

	mov sp, bp 										; Antisave old call frame.
	pop bp
	ret

;;;
;;; WATCOM ARITHMETIC ROUTINES
;;;

global __U4D
__U4D:
	shl edx, 16 									; Upper half of edx.
	mov dx, ax 										; edx = numerator.
	mov eax, edx 									; eax = numerator.
	xor edx, edx

	shl ecx, 16 									; Upper half of ecx.
	mov cx, bx 										; ecx = denominator.

	div ecx 											; eax = quotient, edx = remainder.
	mov ebx, edx
	mov ecx, edx
	shr ecx, 16

	mov edx, eax
	shr edx, 16

	ret

global __U4M
__U4M:
	shl edx, 16 									; Upper half of edx.
	mov dx, ax 										; edx = multipland.
	mov eax, edx 									; eax = multipland.

	shl ecx, 16 									; Upper half of ecx.
	mov cx, bx 										; ecx = multiplier.

	mul ecx 											; Result stored in edx:eax.
	mov edx, eax 									; Move upper half to dx.
	shr edx, 16

	ret

