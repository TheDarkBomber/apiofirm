;;;
;;; APIOFIRM OPERATING SYSTEM: BASIC BOOT LOADER
;;;

org 0x7C00 											; set origin to 0x7C00
bits 16													; explicitly produce 16-bit binary files

%define DOSN 0x0D, 0x0A 				; BIOS TTY mode uses DOS newlines
%define STRD DOSN, 0 						; Ends string with a DOS newline.

;;;
;;; FAT12 HEADER
;;;

jmp short entry
nop

disk_oem:	db 'CRXBOOT1' 				; OEM, 8 bytes.
disk_bytes_psector:	dw 512 			; Bytes per sector.
disk_sectors_pcluster:	db 1 		; Bytes per cluster.
disk_rsvp_sectors:	dw 1 				; Reserved sectors.
disk_file_allocation_table_count:	db 2
disk_directory_entries_count:	dw 0xE0
disk_total_sectors:	dw 2880 		; 2280 = 1440 * 2
disk_media_descriptor_type:	db 0xF0 ; F0 = 3.5 inch
disk_sectors_pfile_allocation_table:	dw 9 ; 9 sectors per file allocation table.
disk_sectors_ptrack:	dw 18
disk_heads:	dw 2
disk_hidden_sectors:	dd 0
disk_large_sector_count:	dd 0

ebr_drive_number:	db 0 					; 0x00 = floppy disk, 0x80 = hard disk
ebr_rsvp:	db 0 									; Reserved.
ebr_signature:	db 0x29
ebr_volume_id:	db 0x12, 0x34, 0x56, 0x78 ; Serial number.
ebr_volume_label:	db 'APIOFIRMCRX'				; Label, 11 bytes.
ebr_system_id:	db 'FAT12   '							; System ID, 11 bytes.

entry:	
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
	mov ax, 0 										; ds and es cannot be altered using direct addressing, set ax to 0, then ds and ex to ax.
	mov ds, ax 										; Data segment.
	mov es, ax 										; Extra data segment.

	mov ss, ax 										; Stack setup.
	mov sp, 0x7C00 								; Origin is 0x7C00, so start stack pointing from there.

	mov [ebr_drive_number], dl 		; DL = drive number, as set by BIOS.

	mov ax, 1 										; LBA = 1
	mov cl, 1 										; Read 1 sector.
	mov bx, 0x7E00 								; Data exists after boot code.
	call diskread 								; Read the disk.

	mov si, message 							; Setup for strput.
	call strput

	jmp exitboot

;;; DISK ERROR
;;; When the disk has an error.

disk_error:
	mov si, disk_read_error_message ; Setup for strput
	call strput
	jmp bootpanic

;;; BOOT PANIC
;;; Waits until user input, then reboots.
bootpanic:
	mov si, bootpanic_message 		; Setup for strput
	call strput
	mov ah, 0
	int 0x16 											; Wait for keypress.
	jmp 0xFFFF:0 									; Jumps to beginning of BIOS, causes a reset.

;;; EXIT BOOT
;;; Halts the CPU, and makes sure of it.
exitboot:
	cli									; Disables interrupts.
	hlt 													; No more execution is to be done.
	jmp $ 												; In case CPU restored from halt, become stagnant forever.


;;; CONVERT CHS
;;; Converts an LBA to CHS.
;;; Parameter: ax
;;; LBA address
;;; Returns: cx, dh
;;; bits 0-5: sector number
;;; bits 6-15: cylinder
;;; head
convchs:
	push ax 											; Saves ax, dx.
	push dx

	xor dx, dx 										; Clears dx.
	div word [disk_sectors_ptrack] ; ax = LBA / Sectors per track

	inc dx 												; dx = sector = (LBA mod Sectors per track) + 1
	mov cx, dx 										; cx = sector

	xor dx, dx 										; Clears dx.
	div word [disk_heads] 				; ax = cylinder =(LBA / Sectors per track) / Heads

	mov dh, dl 										; dh = head
	mov ch, al 										; ch = cylinder
	shl ah, 6
	or cl, ah 										; places upper 2 bits of cylinder in cl

	pop ax 												; Antisave ax, dl
	mov dl, al
	pop ax
	ret

;;; DISK READ
;;; Parameters: ax, cl, dl, es:bx
;;; LBA address
;;; Number of sectors to read
;;; Drive number
;;; Memory address where to store data that is read
diskread:
	push ax 											; Save registers
	push bx
	push cx
	push dx
	push di

	push cx 											; Temporary save cl
	call convchs 									; Convert CHS
	pop ax 												; al = number of sectors to read

	mov ah, 0x02
	mov di, 3 										; Retry attempts
	.retry:
	pusha 												; Save all registers.
	stc 													; Set carry flag.
	int 0x13 											; If carry flag = cleared, ergo success.
	jnc .endretry 								; Jump if this is the case.

	popa 													; Read failed, antisave all registers.
	call diskreset 								; Reset disk.

	dec di
	test di, di
	jnz .retry

	.failure:
	jmp disk_error 								; Disk error.

	.endretry:
	popa 													; Antisave registers.
	pop di
	pop dx
	pop cx
	pop bx
	pop ax
	ret

;;; DISK RESET
;;; Parameter: dl
;;; Drive number
diskreset:
	pusha
	mov ah, 0
	stc
	int 0x13
	jc disk_error
	popa
	ret

;;; variables
message:	db 'Bzzzzzzzzzt.', STRD
disk_read_error_message:	db 'ERR: Could not read disk. Stop.', STRD
bootpanic_message:	db 'A fatal error was encountered during boot.', DOSN, 'Press any key to reboot...', 0

;;; Boot sector magic.
	times 510-($-$$) db 0 				; Pad binary with null until 510th byte.
	dw 0xAA55 										; Boot sector magic number.
