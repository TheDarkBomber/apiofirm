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
