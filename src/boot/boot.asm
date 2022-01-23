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

jmp short startpoint
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

startpoint:
	mov ax, 0 										; Cannot set ds or es via direct addressing.
	mov ds, ax
	mov es, ax

	mov ss, ax
	mov sp, 0x7C00 								; Stack pointer starts at 0x7C00

	push es 											; Assert this is 0x7C00
	push word .after
	retf

	.after:
	mov [ebr_drive_number], dl 		; Read from disk.

	mov si, message 							; Show boot code message.
	call strput

	push es 											; Read drive parameters.
	mov ah, 0x08
	int 0x13
	jc disk_error
	pop es

	and cl, 0x3F 									; Remove upper 2 bits.
	xor ch, ch
	mov [disk_sectors_ptrack], cx ; Sector count.

	inc dh
	mov [disk_heads], dh 					; Head count.

	mov ax, [disk_sectors_pfile_allocation_table] ; Calculate LBA of root.
	mov bl, [disk_file_allocation_table_count]
	xor bh, bh
	mul bx
	add ax, [disk_rsvp_sectors]
	push ax

	mov ax, [disk_directory_entries_count] ; Calculate size of root.
	shl ax, 5
	xor dx, dx
	div word [disk_bytes_psector] ; Number of sectors to read.

	test dx, dx
	jz .root_directory_after
	inc ax

	.root_directory_after:
	mov cl, al
	pop ax
	mov dl, [ebr_drive_number] 		; dl = drive number
	mov bx, buffer 								; es:bx = buffer
	call diskread

	xor bx, bx 										; Find crxboot.kb
	mov di, buffer

	.search_crxboot:
	mov si, crxboot_location
	mov cx, 11 										; FAT12 filenames are 11 characters.
	push di
	repe cmpsb
	pop di
	je .found_crxboot

	add di, 32
	inc bx
	cmp bx, [disk_directory_entries_count]
	jl .search_crxboot

	jmp no_crxboot 								; crxboot.kb not found.

	.found_crxboot:
	mov ax, [di + 26] 						; di has address to entry, first logical cluster field.
	mov [crxboot_cluster], ax

	mov ax, [disk_rsvp_sectors] 	; Load FAT from disk to memory.
	mov bx, buffer
	mov cl, [disk_sectors_pfile_allocation_table]
	mov dl, [ebr_drive_number]
	call diskread

	mov bx, CRXBOOT_LOAD 					; Read crxboot.
	mov es, bx
	mov bx, CRXBOOT_OFFSET

	.load_crxboot:
	mov ax, [crxboot_cluster] 			; Read next cluster.
	add ax, 31

	mov cl, 1
	mov dl, [ebr_drive_number]
	call diskread

	add bx, [disk_bytes_psector]

	mov ax, [crxboot_cluster] 			; Calculate location of next cluster..
	mov cx, 3
	mul cx
	mov cx, 2
	div cx

	mov si, buffer
	add si, ax
	mov ax, [ds:si] 							; Read entry from FAT at index ax.

	or dx, dx
	jz .even

	.not_even:
	shr ax, 4
	jmp .next_cluster_after
	.even:
	and ax, 0x0FFF

	.next_cluster_after:
	cmp ax, 0x0FF8
	jae .read_finish

	mov [crxboot_cluster], ax
	jmp .load_crxboot

	.read_finish:
	mov dl, [ebr_drive_number] 		; Boot device in dl.

	mov ax, CRXBOOT_LOAD 					; Setup segment registers.
	mov ds, ax
	mov es, ax

	jmp CRXBOOT_LOAD:CRXBOOT_OFFSET
	jmp bootpanic 								; Should not occur. (DO NOT RESEARCH)
	jmp exitboot

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

;;; DISK ERROR
;;; When the disk has an error.

disk_error:
	mov si, disk_read_error_message ; Setup for strput
	call strput
	jmp bootpanic

;;; NO CRXBOOT
;;; When the crxboot cannot be found.
no_crxboot:
	mov si, no_crxboot_message
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
message:	db 'bzzzzt c:', STRD
disk_read_error_message:	db 'E: DISK. Stop.', STRD
no_crxboot_message:	db 'E: No CRXBOOT. Stop.', STRD
crxboot_location:	db 'CRXBOOT KB '
crxboot_cluster:	dw 0
bootpanic_message:	db 'ERR!', STRD

	CRXBOOT_LOAD equ 0x2000
	CRXBOOT_OFFSET equ 0

;;; Boot sector magic.
	times 510-($-$$) db 0 				; Pad binary with null until 510th byte.
	dw 0xAA55 										; Boot sector magic number.
buffer:
