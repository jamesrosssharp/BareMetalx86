	org 7C00h
 
	jmp 0000h:Start 
 
Msg:    db "Bootloadin'... "
EndMsg:
 
Start:  
	; set up a stack

	mov ax,0000h
	mov ss, ax
	mov sp, 07c00h

	; preserve drive number

	push dx

	; clear screen

	xor ax,ax
	mov al, 03h

	int 10h
	 
	; display our string

	mov ax, 1301h
	xor dx,dx

	mov es,dx
	mov bp,Msg
		
	mov bx, 000ch
	mov cx, 000fh

	int 10h	


	; read 7 sectors from the disk to 1000:0000 (stage 1.5 loader)

ReadDiskSectors:

	jmp BeginRead

SectorCount:
	dw 0007h

BeginRead:

	mov ax, 1000h
	mov es, ax 

	pop dx	; retrieve drive number
	mov dh,00h	; head

	; starting cylinder and sector
	mov cx,0002h	; ch = cyl = 0, cl = sector = 2

	; starting address
	xor bx,bx	; es = 1000 bx = 0, = 1000:0000 = 10000h

	mov bp,bx

ReadLoop:

	mov ax,0201h	; read 1 sector
	int 13h

	inc cl

	add bx,200h	; add 512 bytes to offset

	dec word [SectorCount]
	jnz ReadLoop

DoneRead:

	push dx

	; print out a string from some address
	; in the image to test everything with
		
	; display our string

	mov ax, 1301h
	
	mov bp, 0005h

	mov dx,0100h
		
	mov bx, 000ch
	mov cx, 0012h

	int 10h	

JumpToSecondStage:

	pop dx

	jmp	1000h:0000h


End:
	times 01BEh - ($ - $$)  db 0    ;Zerofill up to partition entries

Partition0:
	db 080h	; bootable
	db 21h	;	 CHS
	db 03h
	db 00h
	db 83h	; Linux
	db 41h	; CHS end
	db 0e8h
	db 0fdh
	db 00h	; LBA
	db 08h
	db 00h
	db 00h
	db 40h	; LBA
	db 0e2h
	db 0e6h
	db 00h

Partition1:
	times 16 db 0

Partition2:
	times 16 db 0

Partition3:
	times 16 db 0

	dw 0AA55h       ;Boot Sector signature

; Start of sector stage bootloader
	incbin	"../stage1_5.bin"
	
	times 1000h - ($ - $$)  db 0 

; Payload starts at 4k
 
; Start of payload (kernel)

	incbin	"../payload.bin"
 
; make it we generate the first 1 Meg, which is reserved by partition table.
; need to write this to an image, and write filesystem etc.
 times 1024*1024*1 - ($ - $$) db 0
