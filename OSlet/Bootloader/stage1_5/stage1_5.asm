
	bits 16	;16 bit code

%include "FATdefs.h"

	org 	0000h
	jmp	1000h:start	; long jump to start

	db	"Stage 1.5 Loaded.."
startMsg:
	db	"Stage 1.5 Running..."

start:
	
	; set the data segment appropriately
	push cs
	pop  ax

	mov  ds,ax
	mov  es,ax

	; first stage will pass drive in dx
	
	mov [DriveNumber], dx
	
	; display start string

	mov dx, 0200h
	mov bp,startMsg
	mov cx, 0014h	
	
	call PrintString

	;	Query drive geometry. We will use this to load
	;	from 4k our payload

	mov dx, [DriveNumber]

	mov ah,08h

	push es

	int 13h

	pop es

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
	mov al, [MaxHeads]
	and al, 63

	call PrintHexByte

	mov dx, 0418h
	mov al, [MaxSectors]

	call PrintHexByte

	;==============================================================
	;                Determine FAT partition C,H,S offset
	;==============================================================

	; since we have come in from the MBR, the MBR is still loaded
	; at 0x7c00. We can use this to parse the parition table,
	; parsing each entry in turn until we get a FAT16 partition.
	; this is the boot partition. Get the CHS offset of it.

	xor ax, ax
	mov es, ax

	mov di, 7dbeh

	mov cx, 4

FindPartitionLoop:	

	mov al, [es:di + 4]
	cmp al, 6	; FAT16 partition

	je  PartitionFound

	add di, 16

	loop FindPartitionLoop

	; display a string... could not find the partition	

	push cs
	pop es

	mov dx, 0500h
	mov bp,	CouldNotFindPartitionString
	mov cx, [CouldNotFindPartitionStringLen]

	call PrintString

	jmp EndLoop16

PartitionFound:

	push es
	push di
	push cx

	push cs
	pop es

	mov dx, 0500h
	mov bp,	FoundPartitionString
	mov cx, [FoundPartitionStringLen]

	call PrintString

	pop cx

	mov dx, 051ch
	mov al, 5
	sub al, cl

	call PrintHexByte

	pop di

	; get address of partition in logical units

	pop es

	mov eax, [es:di + 8]

	push eax

	push cs
	pop  es

	mov cl, 4

	mov dx, 0600h

PrintLBA:

	call PrintHexByte

	shr eax,8
	add dx,4

	loop PrintLBA

	; convert to CHS based on geometry

	pop eax

	xor edx, edx

	mov ecx,[MaxSectors]
	div ecx

	inc edx
	mov [FATPartitionS], edx

	push eax

	mov al, dl
	mov dx, 0700h

	call PrintHexByte

	pop eax
	xor edx,edx
	
	mov ecx,[MaxHeads]
	inc ecx

	div ecx

	mov [FATPartitionH], edx
	mov [FATPartitionC], eax

	mov al,[FATPartitionH]
	mov dx, 0704h

	call PrintHexByte

	mov al,[FATPartitionC]
	mov dx, 0708h

	call PrintHexByte

	;==============================================================
	;                Load FAT partition to 0x2000:0x0000
	;	- and check that this partition is a FAT16 partition	
	;==============================================================

LoadFATPartition:

	mov bx, BPB

	mov ax, 1

	mov dx, [DriveNumber]

	mov dh, [FATPartitionH]

	mov cl, [FATPartitionS]
	mov ch, [FATPartitionC]

	call ReadDiskSectors

	; print OEM name

	mov dx, 0800h
	mov bp,	BPB + FAT_BS_VolLab	
	mov cx, 11

	call PrintString

	; compute number of clusters on disk

	; 1. compute RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;		

	mov di,BPB
	mov ax, [es:di + FAT_BPB_RootEntCnt]

	shl ax, 5

	xor dx, dx
	mov cx, [es:di + FAT_BPB_BPB_BytsPerSec]
	add ax, cx
	dec ax

	div cx  

	mov [FATRootSectors], ax	

	;	If(BPB_FATSz16 != 0)
	;		FATSz = BPB_FATSz16;
	;	Else
	;		FATSz = BPB_FATSz32;
	;	If(BPB_TotSec16 != 0)
	;		TotSec = BPB_TotSec16;
	;	Else
	;		TotSec = BPB_TotSec32;
	;	DataSec = TotSec – (BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors);

	cmp word [es:di + FAT_BPB_FATSz16], 0
	je  FAT16NotFound

	xor eax, eax

	cmp word [es:di + FAT_BPB_TotSec16], 0
	je .Use32BitSectors

	mov ax, [es:di + FAT_BPB_TotSec16]
	jmp .GotSectors

	.Use32BitSectors:

	mov eax, [es:di + FAT_BPB_TotSec32]

	.GotSectors:

	xor ecx, ecx
	mov cx,  [es:di + FAT_BPB_RsvdSecCnt]

	sub eax, ecx

	mov ebx, eax

	xor eax, eax
	mov ax, [es:di + FAT_BPB_FATSz16]
	xor cx, cx
	mov cl, [es:di + FAT_BPB_NumFATs]

	mul cx

	mov [FATSectors], ax
	mov [FATSectors + 2], dx 	

	mov eax, ebx
	sub eax, [FATSectors]

	xor ebx, ebx
	mov bx, [FATRootSectors]
	sub eax, ebx

	xor edx, edx
	xor ecx, ecx

	mov cl, [es:di + FAT_BPB_SecPerClus]
	div ecx

	mov [FATClusters], eax		

	; Print FAT cluster count

	mov dx, 0900h
	mov cx, 4

	.PrintClusters:
	call PrintHexByte
	shr eax, 8
	add dx, 4
	loop .PrintClusters

	mov eax, [FATClusters]

	; FAT16 partition?

	cmp eax, 4085
	jl FAT16NotFound

	cmp eax, 65525
	jl FAT16Verified

	; else ExFAT, FAT32 etc.

FAT16NotFound:

	mov dx, 0a00h
	mov bp,	FAT16NotFoundString
	mov cx, [FAT16NotFoundStringLen]

	call PrintString

	jmp EndLoop16

FAT16Verified:

	;==============================================================
	;                Load root directory at 0x20000
	;==============================================================

LoadRootDirectory:
	
	; compute root directory offset for FAT16
	;	- FirstRootDirSecNum = BPB_ResvdSecCnt + (BPB_NumFATs * BPB_FATSz16);
	
	xor eax,eax
	mov ax, [es:di + FAT_BPB_RsvdSecCnt]
	add eax, [FATSectors]

	mov dx, [DriveNumber]

	; add root dir sectors to FATPartitionC, H, S

	call ComputeCHSFromFATPartition

	mov ax, 2000h
	mov es, ax

	xor bx, bx
	mov ax, [FATRootSectors]

	call ReadDiskSectors

	;==============================================================
	;                Find boot directory and load at 0x20000
	;==============================================================

FindBootDirectory:

	xor ax, ax
	mov di, ax

	mov si, BootDirectoryName

	mov cx, 10h	; look at the first 16 directory entries. One must
				; be boot
		
	.FindLoop:

	push cx
	push di
	push si

	mov cx, 11	; sizeof dir name in FAT

	call CompareStrings

	pop si
	pop di
	pop cx

	je .BootDirectoryFound

	add di, 32 ; size of directory entry	

	loop .FindLoop

	; Boot wasn't in the first 16 directory entries

	push cs
	pop  es

	mov dx, 0b00h
	mov bp,	BootDirNotFoundString
	mov cx, [BootDirNotFoundStringLen]

	call PrintString

	jmp EndLoop16

	.BootDirectoryFound:
	
	; get cluster offset of boot directory

	xor eax, eax
	mov ax, [es:di + FAT_DIR_FstClusLO]

	; compute sector offset from partition

	call ComputeFATSectorOffset	; returns sector offset (of cluster in ax) in eax	

	; compute absolute sector offset

	mov dx, [DriveNumber]

	call ComputeCHSFromFATPartition

	;push es 

	;push cs
	;pop es

	;xor eax, eax

	;mov al, ch
	;shl eax,8
	;mov al, dh
	;shl eax, 8
	;mov al, cl

	;mov cx, 4
	;mov dx, 0a00h

	;.PrintFATOffset:
	;call PrintHexByte

	;shr eax,8
	;add dx,4

	;loop .PrintFATOffset

	;pop es

	;jmp EndLoop16

	; read the directory cluster to 0x2000:0x0000 

	xor eax, eax

	mov al, [BPB + FAT_BPB_SecPerClus]
	xor bx, bx

	call ReadDiskSectors

	;==============================================================
	;                Find LOADER.BIN first cluster
	;==============================================================

FindLoaderBin:

	xor ax, ax
	mov di, ax

	mov si, LoaderName

	mov cx, 10h	; look at the first 16 directory entries.
		
	.FindLoop:

	push cx
	push di
	push si

	mov cx, 11	; sizeof dir name in FAT

	call CompareStrings

	pop si
	pop di
	pop cx

	je .LoaderFound

	add di, 32 ; size of directory entry	

	loop .FindLoop

	; Boot wasn't in the first 16 directory entries

	push cs
	pop  es

	mov dx, 0a00h
	mov bp,	LoaderNotFoundString
	mov cx, [LoaderNotFoundStringLen]

	call PrintString

	jmp EndLoop16

	.LoaderFound:

	; get cluster offset of LOADER.BIN

	xor eax, eax
	mov ax, [es:di + FAT_DIR_FstClusLO]

	mov [LoaderBinCluster], ax
	
	;==============================================================
	;		 Load FAT at 0x20000
	;==============================================================

LoadFAT:

	xor eax, eax
	mov ax, [BPB + FAT_BPB_RsvdSecCnt]

	mov dx, [DriveNumber]

	; add root dir sectors to FATPartitionC, H, S

	call ComputeCHSFromFATPartition

	mov ax, 2000h
	mov es, ax

	xor bx, bx
	mov ax, [BPB + FAT_BPB_FATSz16]

	call ReadDiskSectors

	;==============================================================
	;                Load LOADER.BIN at 0x30000
	;	Note: loader max size, 0x60000 (393216 bytes)
	;==============================================================

LoadLoader:

	mov dx, [LoaderBinCluster]	; dx contains cluster

	mov ax, es
	mov fs, ax	; fs contains segment of FAT

	mov ax, 3000h
	mov es, ax	; es contains segment of data to load

	xor bx,bx

	.LoadLoop:

	push dx
	push bx

	xor eax, eax
	mov ax, dx

	call ComputeFATSectorOffset	; returns sector offset (of cluster in ax) in eax	
	
	mov dx, [DriveNumber]
	
	call ComputeCHSFromFATPartition

	xor eax, eax
	mov al, [BPB + FAT_BPB_SecPerClus] 

	pop bx

	call ReadDiskSectors

	pop dx

	mov ax, dx
	shl ax, 1
	
	mov di, ax
	mov dx, [fs:di]

	cmp dx, 0xfff8	; EOC?

	jb .LoadLoop

	;==============================================================
	;                Shift loader down to 0x20000
	;==============================================================

ShiftLoader:
	
	mov ax, 3000h
	mov ds, ax

	mov ax, 2000h
	mov es, ax

	mov cx, 6 ; move 6 segments (0x3000 - 0x8FFFF)

	xor ax, ax
	mov di, ax
	mov si, ax

	.Outer:

	push cx
	mov cx, 4000h	; 16k dwords

	rep movsd

	pop cx

	mov ax, es
	add ax,1000h
	mov es, ax

	mov ax, ds
	add ax,1000h
	mov ds, ax

	loop .Outer
	
	push cs
	pop  ax
	mov es, ax
	mov ds, ax

	;==============================================================
	;                Ready to enter protected mode and jump
	;		 to the loader
	;==============================================================

ProgramLoaded:

	push cs
	pop  es

	; display a message that the c program was loaded

	mov dx, 0b00h
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
	mov dx, 0c00h
	mov bp,	A20FailedString
	mov cx, [A20FailedStringLen]

	call PrintString

	jmp EndLoop ; die
A20Done:

	mov dx, 0c00h
	mov bp,	A20GoodString
	mov cx, [A20GoodStringLen]

	call PrintString
	
	; wait 10 seconds

	mov cx,0098h
	mov dx,9680h	
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
	
	; make doubly sure interrupts are disabled

	cli

	; go into protected mode

	mov eax, cr0
	or  al,1
	mov cr0, eax

	; ============= START 32 BIT CODE ===================


	[BITS 32]

	jmp ClearPrefetchQueue	; not sure this is necessary...
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
	jmp EndLoop16


	
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
	;		es: 	segment

PrintString:
	push ax
	push bx

	mov ax, 1301h
	mov bx, 000ch

	int 10h	

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

	je  .RetA20 ; es:[di] was 0xff, memory accesses wrapped, so A20
		   ; must not be enabled. Return non zero.

	xor ax,ax ; es:[di] was zero. Memory accesses did not wrap. Return zero.

	.RetA20:

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

	;
	;	Function: ReadDiskSectors
	;
	;	In: ax = Sector count
	;	    es:bx = buffer to read into to
	;	    dh    = head
	;	    dl    = drive number
	;	    ch    = cylinder
	;	    cl    = sector	
	;	Out:
	;	    bx    = pointer to data
	;	    es    = segment of data 

ReadDiskSectors:
	push ax
	push cx
	push dx

	push di
	push si
	push bp

	mov [.SectorCount], ax	

	jmp .ReadLoop

	.SectorCount:
		dw 0h ; read 640 sectors



	.ReadLoop:

	mov ax,0201h	; read 1 sector
	int 13h

	inc cl
	cmp byte cl,[MaxSectors]
	
	jle .SkipClear

	mov cl,01h

	inc dh	;increment head

	cmp byte dh,[MaxHeads]

	jle  .SkipClear

	mov dh, 0

	inc ch

	.SkipClear:

	add bx,200h	; add 512 bytes to offset

	jnc	.SkipIncSeg

	push ax

	mov ax, es
	add ax, 1000h
	mov es, ax

	pop ax

	.SkipIncSeg:

	dec word [.SectorCount]
	jnz .ReadLoop

	pop bp
	pop si
	pop di

	pop dx
	pop cx
	pop ax

	ret

	;	Function: ComputeCHSFromFATPartition
	;
	;	In: eax = sector count 
	;
	;	Out:
	;		dh = head
	;		ch = cylinder
	;		cl = sector
	;
	;	Trashes: eax

ComputeCHSFromFATPartition:
	mov dh, [FATPartitionH]
	mov ch, [FATPartitionC]
	mov cl, [FATPartitionS]

	.incLoop:

	inc cl
	cmp byte cl,[MaxSectors]
	
	jle .SkipClear

	mov cl,01h

	inc dh	;increment head

	cmp byte dh,[MaxHeads]

	jle  .SkipClear

	mov dh, 0

	inc ch

	.SkipClear:

	dec eax

	jnz .incLoop

	ret

	;
	;
	;	Function: CompareStrings
	;
	;	In:
	;		ds:si: string a
	;		es:di: string b
	;		cx:    string length
	;	Out:
	;		sets zero flag if equal
	;

CompareStrings:
	.CmpLoop:
	cmpsb
	jnz .Exit
	loop .CmpLoop

	.Exit:
	ret

	;
	;	Function: ComputeFATSectorOffset
	;
	;	In:
	;		eax:  cluster number	
	;	Out:
	;		eax: offset in sectors from partition start

ComputeFATSectorOffset:
	push ebx
	push ecx
	push edx

	xor ebx,ebx

	mov bx, [BPB + FAT_BPB_RsvdSecCnt]  	

	add ebx, [FATSectors]
	add ebx, [FATRootSectors]

	sub eax, 2	; clusters - 2

	xor edx, edx	
	xor ecx, ecx

	mov cl, [BPB + FAT_BPB_SecPerClus]

	mul ecx

	add eax, ebx

	pop edx	
	pop ecx
	pop ebx

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

CouldNotFindPartitionString:
	db "Could not find FAT16 Boot partition"
CouldNotFindPartitionStringLen:
	dw CouldNotFindPartitionStringLen - CouldNotFindPartitionString

FoundPartitionString:
	db "Found FAT16 Boot partition:"
FoundPartitionStringLen:
	dw FoundPartitionStringLen - FoundPartitionString

FAT16NotFoundString:
	db "Boot partition not FAT16: Incorrect no. of clusters"
FAT16NotFoundStringLen:
	dw  FAT16NotFoundStringLen - FAT16NotFoundString

BootDirNotFoundString:
	db "Boot Directory not found"
BootDirNotFoundStringLen:
	dw BootDirNotFoundStringLen - BootDirNotFoundString

LoaderNotFoundString:
	db "LOADER.BIN not found"
LoaderNotFoundStringLen:
	dw LoaderNotFoundStringLen - LoaderNotFoundString

BootDirectoryName:
	db "BOOT       "

LoaderName:
	db "LOADER  BIN"

	align 4
DriveNumber:
	dw	0
MaxCylinders:
	dd	0
MaxHeads:
	dd	0
MaxSectors:
	dd	0

FATPartitionS:
	dd 	0
FATPartitionH:
	dd	0
FATPartitionC:
	dd	0
FATRootSectors:
	dd	0
FATSectors:
	dd	0
FATClusters:
	dd	0
LoaderBinCluster:
	dd	0


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

	align 512

BPB:
	times 512 db 0

