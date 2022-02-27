;;;
;;; APIOFIRM OPERATING SYSTEM: BOOT STUB FOR NON-BOOTABLE DISKS
;;;

org 0x7C00 											; set origin to 0x7C00
bits 16													; explicitly produce 16-bit binary files

%define DOSN 0x0D, 0x0A 				; BIOS TTY mode uses DOS newlines
%define STRD DOSN, 0 						; Ends string with a DOS newline.

;;;
;;; FAT12 HEADER
;;;

jmp short startpoint
nop

disk_oem:	db 'APIOFIRM' 				; OEM, 8 bytes.
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
ebr_volume_label:	db '1_44 FLOPPY'				; Label, 11 bytes.
ebr_system_id:	db 'FAT12   '							; System ID, 11 bytes.

startpoint:
	mov ax, 0 										; Cannot set ds or es via direct addressing.
	mov ds, ax
	mov es, ax

	mov ss, ax
	mov sp, 0x7C00 								; Stack pointer starts at 0x7C00

	mov si, message 							; Show boot information message.
	call strput

	jmp exitdisk 								  ; Should occur. (RESEARCH)

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

;;; EXIT DISK
;;; Waits until user input, then reboots.
exitdisk:
	mov ah, 0
	int 0x16 											; Wait for keypress.
	jmp 0xFFFF:0 									; Jumps to beginning of BIOS, causes a reset.
	cli
	hlt
	jmp $

;;; variables
message:	db DOSN, 'This is not a bootable floppy disk.', DOSN, 'This disk contains a FAT12 filesystem for regular data.', DOSN, 'Please remove this disk and insert a bootable disk if applicable.', DOSN, DOSN, 'Press any key to reboot.', STRD

;;; Boot sector magic.
	times 510-($-$$) db 0 				; Pad binary with null until 510th byte.
	dw 0xAA55 										; Boot sector magic number.
