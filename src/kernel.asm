;;;
;;; APIOFIRM OPERATING SYSTEM: KERNEL
;;;

org 0x0 											; set origin to 0x0
bits 16													; explicitly produce 16-bit binary files

%define DOSN 0x0D, 0x0A 				; BIOS TTY mode uses DOS newlines
%define STRD DOSN, 0 						; Ends string with a DOS newline.

	jmp main 											; Jump to main, unconditionally.

;;; STRPUT
;;; Outputs a string to BIOS TTY.
;;; Parameter: ds:si
;;; Stores pointer to string.

strput:
	push si 											; Save registers si, ax, and bx.
	push ax
	push bx
	.loop:
	lodsb 												; Load next character, into al.
	or al, al 										; Checks if al = 0
	jz .put 											; If so, jump to put.

	mov ah, 0x0E 									; Set BIOS TTY mode.
	mov bh, 0x00 									; Page 0.
	int 0x10 											; Print character.
	jmp .loop 										; Loop back.

	.put:
	pop bx 												; Antisave registers si, ax, and bx.
	pop ax
	pop si
	ret

;;; MAIN
;;; Main entrypoint function. Does not return.

main:
	mov si, k_message 							; Setup for strput.
	call strput

stop_sys:
	cli
	hlt 													; No more execution is to be done.
	jmp $ 												; In case CPU restored from halt, become stagnant forever.

;;; variable
k_message:	db 'Bzzzzzzzzzt.', DOSN, 'Welcome to kernelspace.', STRD
