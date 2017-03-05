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

	; read sectors from the disk to 1000:0000
	; to do this we need to loop with ch containing the 
	; cylinder number, cl containing the sector, dh containing
	; the head. We increment cl until it equals 19, then we clear it
	; and increment the head until it equals 2, then we clear that
	; and increment the cylinder number, until 128 sectors have 
	; been read into memory.

ReadDiskSectors:

	jmp BeginRead

SectorCount:
	dw 0080h

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
	cmp cl,18
	
	jle SkipClear

	mov cl,01h

	inc dh

	cmp dh,01h
	jle SkipClear

	mov dh,00h

	inc ch

SkipClear:

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
	times 0200h - 2 - ($ - $$)  db 0    ;Zerofill up to 510 bytes

	dw 0AA55h       ;Boot Sector signature

; Start of sector stage bootloader
	incbin	"secondstage.bin"
	
	times 10000h + 200h - ($ - $$)  db 0 
 
; Start of payload c program

	incbin	"payload.bin"
 

 ;OPTIONAL:
 ;To zerofill up to the size of a standard 1.44MB, 3.5" floppy disk
 times 1474560 - ($ - $$) db 0
