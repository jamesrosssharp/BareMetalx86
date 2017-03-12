	bits 16	;16 bit code

	org 	0000h
	jmp	1000h:start	; long jump to start

	db	"SecondStage Loaded"
startMsg:
	db	"SecondStage Running..."

start:

	; first stage will pass drive in dx
	push dx

	; set the data segment appropriately
	push cs
	pop  ds
	
	; display start string

	mov dx, 0200h
	mov bp,startMsg
	mov cx, 0016h	
	
	call PrintString

	;	Query drive geometry. We will use this to load
	;	from 4k our payload

	pop  dx		; get drive number
	push dx

	mov ah,08h

	int 13h

	jc  BIOSCallFailed

	and cl, 63

	mov byte [MaxCylinders], ch	
	mov byte [MaxSectors], cl
	mov byte [MaxHeads], dh


	mov dx, 0400h
	mov bp,	DiskGeoString
	mov cx, [DiskGeoStringLen]

	call PrintString

	mov dx, 0410h
	mov al, [MaxCylinders]

	call PrintHexByte

	mov dx, 0414h
	mov al, [MaxSectors]
	and al, 63

	call PrintHexByte

	mov dx, 0418h
	mov al, [MaxHeads]

	call PrintHexByte

	;jmp EndLoop16


	; now we want to load in our payload c program into memory at 2000:0000
	; we can load 327680 bytes (=640 sectors) starting from 4k ( CHS (0,0,9) )

ReadDiskSectors:

	jmp BeginRead

SectorCount:
	dw 280h ; read 640 sectors

BeginRead:

	mov ax, 2000h
	mov es, ax 

	pop dx
	mov dh,0h	; head

	; starting cylinder and sector
	mov cx,0009h	; ch = cyl = 3, cl = sector = 4

	; starting address
	xor bx,bx	; es = 2000 bx = 0, = 2000:0000 = 20000h

	mov bp,bx

ReadLoop:

	mov ax,0201h	; read 1 sector
	int 13h

	inc cl
	cmp byte cl,[MaxSectors]
	
	jle SkipClear

	mov cl,01h

	inc dh	;increment head

	cmp byte dh,[MaxHeads]

	jl  SkipClear

	mov dh, 0

	inc ch

SkipClear:

	add bx,200h	; add 512 bytes to offset

	jnc	SkipIncSeg

	push ax

	mov ax, es
	add ax, 1000h
	mov es, ax

	pop ax

SkipIncSeg:

	dec word [SectorCount]
	jnz ReadLoop

DoneRead:

	; display a message that the c program was loaded

	mov dx, 0300h
	mov bp,	ProgLoadedString
	mov cx, 0011h

	call PrintString

	;============================================
	;	Enable A20
	;============================================

	; disable interrupts

	cli

	; A20 may be enabled on QEMU. Disable it with BIOS call to test enabling

	;mov ax, 2400h
	;int 15h

	; attempt to enable A20 gate

	call TestA20

	or al,al
	jz A20Done

	; 1. attempt to enable via bios

	mov ax, 2401h
	int 15h

	mov byte [A20GoodStringLen - 1], '1' ; display method used to enable A20

	call TestA20

	or al,al
	jz A20Done	

	; 2. attempt to enable via keyboard controller

	call A20KbdEnable

	mov byte [A20GoodStringLen - 1], '2'

	call TestA20
	
	or al,al
	jz A20Done

	; 3. attempt to enable via Fast A20

	call A20FastEnable

	mov byte [A20GoodStringLen - 1], '3'

	call TestA20

	or al,al
	jz A20Done

	; A20 Gate enabling failed. Print error and terminate.

A20Failed:
	
	mov dx, 0500h
	mov bp,	A20FailedString
	mov cx, [A20FailedStringLen]

	call PrintString

	jmp EndLoop ; die

A20Done:

	mov dx, 0500h
	mov bp,	A20GoodString
	mov cx, [A20GoodStringLen]

	call PrintString
	
	; wait 1 second

	mov cx,000fh
	mov dx,8240h	; was 4240
	mov ax,8600h
	int 15h	

	;======================================================
	;	Begin entry into protected mode
	;======================================================

	; move GDT to nice address

	mov si, gdt
	xor ax,ax
	mov es,ax
	mov di, 500h
	
	mov cx, gdt_ptr - gdt

	rep movsb	

	; Load GDT and IDT

	lidt [idt_ptr]
	lgdt [gdt_ptr]
	
	; initialise all segments

	; go into protected mode

	mov eax, cr0
	or  al,1
	mov cr0, eax

	; ============= START 32 BIT CODE ===================


	[BITS 32]

	jmp ClearPrefetchQueue
	nop
	nop
ClearPrefetchQueue:

	; long jump to C program start
	

	db 066h		; for some reason, the disassembly is incorrect
	jmp 08h:20000h

	; should never get here

EndLoop:
	hlt
	jmp EndLoop

	[BITS 16]

BIOSCallFailed:
	mov dx, 0400h
	mov bp, BIOSCallFailedString	
	mov cx, [BIOSCallFailedStringLen]

	call PrintString

EndLoop16:
	hlt
	jmp EndLoop


	
; =============================================================
;	Functions
; =============================================================


;
;	PrintString
;
;	In:
;		bp:	address of string (relative to code segment)
;		dx:	row and column to write string to
;		cx:	string length

PrintString:
	push ax
	push bx
	push es

	mov ax, 1301h

	mov bx,cs
	mov es,bx
		
	mov bx, 000ch

	int 10h	

	pop es
	pop bx
	pop ax

	ret

;
;	Function:	PrintHexByte
;
;	In:	dx = row and column to print
;		al = byte to print

PrintHexByte:

	push si
	push bp
	push bx
	push ax
	push cx

	xor ah,ah

	push ax

	and al, 0fh
	
	; convert first nibble
	mov bx, HexConversionTable
	add bx, ax
	mov si, bx
	mov byte cl, [ds:si]
	mov byte [HexBuffer + 1], cl
	
	; convert second nibble

	pop ax

	shr ax, 4
	
	mov bx, HexConversionTable
	add bx, ax
	mov si, bx
	mov byte cl, [ds:si]
	mov byte [HexBuffer], cl

	mov bp, HexBuffer
	mov cx, 0002h
	
	call PrintString	

	pop cx
	pop ax
	pop bx
	pop bp
	pop si

	ret

;
;
;	Function: TestA20
;
;	In: None
;	Out: al = 0, A20 enabled
;		= 1, A20 not enabled
;

TestA20:

	push es
	push ds
	push di
	push si

	xor ax,ax
	mov es,ax
	mov di,500h

	not ax
	mov ds,ax
	mov si,510h


	mov byte [es:di], 00h

	mov byte [ds:si], 0ffh

	; If A20 is not enabled, es:[di] and ds:[si] point to the 
	; same address in RAM (500h is free ram available for access)

	cmp byte [es:di], 0ffh

	je  RetA20 ; es:[di] was 0xff, memory accesses wrapped, so A20
		   ; must not be enabled. Return non zero.

	xor ax,ax ; es:[di] was zero. Memory accesses did not wrap. Return zero.

RetA20:

	pop si
	pop di
	pop ds
	pop es
	ret

;
;	Function: Wait8042Command
;
;	In: None
;	Out: None
;	Trashes: ax
;

Wait8042Command:
	in      al,64h
  	test    al,2
  	jnz     Wait8042Command
  	ret

;
;	Function: Wait8042Data
;
;	In: None
;	Out: None
;	Trashes: ax
;

Wait8042Data:
	in      al,64h
  	test    al,1
  	jz      Wait8042Data
  	ret

;
;	Function: A20KbdEnable
;
;	In: None
;	Out: None
;

A20KbdEnable:
	push ax

	call    Wait8042Command  
 	mov     al,0adh            ; Send command 0xad (disable keyboard).
   	out     64h,al
 
	call    Wait8042Command  
 	mov     al,0d0h            ; Send command 0xd0 (read from input)
 	out     64h,al
 
 	call    Wait8042Data    
    	in      al,60h            ; Read input from keyboard
    	push    ax                 ; ... and save it
 
    	call    Wait8042Command    
    	mov     al,0d1h            ; Set command 0xd1 (write to output)
    	out     64h,al            
 
    	call    Wait8042Command 
    	pop     ax                ; Write input back, with bit #2 set
    	or      al,2
    	out     60h,al
 
   	call    Wait8042Command  
    	mov     al,0aeh            ; Write command 0xae (enable keyboard)
    	out     64h,al
 
    	call    Wait8042Command  ; Wait until controller is ready for command
 
	pop ax

	ret

;
;	Function: A20FastEnable
;
;	In: None
;	Out: None
;

A20FastEnable:
	push ax

	in al, 92h
	or al, 2
  	out 92h, al

	pop ax
	ret

;==============================================================
;	Data
;==============================================================

ProgLoadedString:
	db "Payload loaded..."

A20FailedString:
	db "Enabling A20 Failed. Aborting."
A20FailedStringLen:
	dw A20FailedStringLen - A20FailedString

A20GoodString:
	db "Enabled A20: 0"
A20GoodStringLen:
	dw A20GoodStringLen - A20GoodString

BIOSCallFailedString:
	db "BIOS Call failed!"
BIOSCallFailedStringLen:
	dw BIOSCallFailedStringLen - BIOSCallFailedString 

DiskGeoString:
	db "Disk Geometry:"
DiskGeoStringLen:
	dw DiskGeoStringLen - DiskGeoString


MaxCylinders:
	db	0
MaxHeads:
	db	0
MaxSectors:
	db	0


gdt:
	dw 0,0,0,0	; null descriptor

	; code selector
	dw 00fffh	; 4GB limit
	dw 00000h
	dw 9a00h	; executable, read only
	dw 00cfh

	; data selector
	dw 0ffffh
	dw 00000h
	dw 9200h
	dw 00cfh

gdt_ptr: 
	dw gdt_ptr - gdt - 1
	dd 500h

idt_ptr:	; dummy idt for now (c program can reconfigure interrupt
		; descriptor if it needs to)
	dw 0
	dw 0,0

HexBuffer:
	dw 0

HexConversionTable:
	db '0'
	db '1'
	db '2'
	db '3'
	db '4'
	db '5'
	db '6'
	db '7'
	db '8'
	db '9'
	db 'a'
	db 'b'
	db 'c'
	db 'd'
	db 'e'
	db 'f'



