
	bits 32

	extern main
	global start

	global boot_realModeIntWrapper

	global boot_driveNumber

	; important constants

KERNEL_SEG	equ	02000h
KERNEL_OFFSET	equ	KERNEL_SEG*10h

;======================== Entry from Bootloader	==============================

kernel_entry_point:
start:

	mov eax, 00000010h
	mov ds, ax

	; We don't trust the bootloader's idt and gdt.
	; Load our own here.

	lidt [kernel_idt_ptr]
	lgdt [kernel_gdt_ptr]

	; Long jump to set cs segment

	jmp	08h:set_cs	

set_cs:

	; rest of init

	mov eax, 00000010h
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax
	mov ss, ax
	mov esp, 0008fff0h	; Move the stack well away from the EBDA typical location

	finit			; Should really do this once we have detected
				; that the CPU has an FPU


	mov [boot_driveNumber], edx ; preserve drive number


	cld			; clear direction flag

	jmp	main

;======================		Kernel gdt and idt

kernel_gdt:
	dw 0,0,0,0	; null descriptor

	; code selector
	dw 00ffffh	; 4GB limit
	dw 00000h
	dw 9a00h	; executable, read only
	dw 00cfh

	; data selector
	dw 0ffffh
	dw 00000h
	dw 9200h
	dw 00cfh

	; code selector for transition into real mode
	dw 0ffffh
	dw 00000h
	dw 9a00h
	dw 0000fh

	; selector for transition into real mode	
	dw 0ffffh 
	dw 00000h
	dw 09200h	; access = 0a2h = RW | PR
	dw 0000fh	; flags = 0h = 16 bit, byte granularity 

kernel_gdt_ptr: 
	dw kernel_gdt_ptr - kernel_gdt - 1
	dd kernel_gdt

kernel_idt_ptr:	
	dw 0
	dw 0,0

realmode_idt_ptr:
	dw 400h
	dw 0,0

boot_driveNumber:
	dd 0

;=======================	Real Mode int call wrapper	================

	align	4

RealModeWrapperStart:
InRegisters:	; the format of InRegisters must follow that in IO/realmodeint.h
	InEAX:		dd	0		;unsigned int EAX;
	InEBX:		dd	0		;unsigned int EBX;
	InECX:		dd	0		;unsigned int ECX;
	InEDX:		dd	0		;unsigned int EDX;

	InDS:		dw	0		;unsigned short DS;
	InES:		dw	0		;unsigned short ES;
	InFS:		dw	0		;unsigned short FS;
	InGS:		dw	0		;unsigned short GS;

	InESI:		dd	0		;unsigned int ESI;
	InEDI:		dd	0		;unsigned int EDI;
	InEBP:		dd	0		;unsigned int EBP;
EndInRegisters:

OutRegisters:	; the format of OutRegisters must follow that in IO/realmodeint.h
	OutEAX:		dd	0		;unsigned int EAX;
	OutEBX:		dd	0		;unsigned int EBX;
	OutECX:		dd	0		;unsigned int ECX;
	OutEDX:		dd	0		;unsigned int EDX;

	OutDS:		dw	0		;unsigned short DS;
	OutES:		dw	0		;unsigned short ES;
	OutFS:		dw	0		;unsigned short FS;
	OutGS:		dw	0		;unsigned short GS;

	OutESI:		dd	0		;unsigned int ESI;
	OutEDI:		dd	0		;unsigned int EDI;
	OutEBP:		dd	0		;unsigned int EBP;
EndOutRegisters:

OutRegistersAddress:
		dd	0
BIOSCarryFlag:
		dd	0
ProtectedModeStackPointer:
		dd	0
ProtectedModeStackSegment:
		dd	0

	align	4

;	int boot_realModeIntWrapper(unsigned char interrupt, 
;				    struct RegisterDescription *inRegisters, 
;				    struct RegisterDescription *outRegisters)

boot_realModeIntWrapper:

	push   ebp
  	mov    ebp,esp

	; save all registers (we are allowed to trash edx and eax)

	push   ebx
	push   ecx
	push   edi
	push   esi
	push   ds
	push   es
	push   fs
	push   gs
 
	;	Copy in registers to register structure

	mov	eax, [ebp + 0ch]
	mov	esi, eax

	mov	edi, InRegisters
	
	mov	ecx, (EndInRegisters - InRegisters) / 4

	rep	movsd

	;	Store out registers address

	mov	eax, [ebp + 10h]
	mov	[OutRegistersAddress], eax

	;	Store interrupt number 
	mov	eax, [ebp + 08h]
	mov	[RealModeExecInt + 1], al

	;	Preserve stack pointer and stack segment

	mov	ax,ss
	mov	[ProtectedModeStackSegment], ax

	mov	eax, esp
	mov	[ProtectedModeStackPointer], eax

	; 	Clear BIOS carry flag (stored DWORD)

	xor	eax,eax
	mov	[BIOSCarryFlag], eax

	; self-modify the address to jump to

	mov	eax, RealModeEnter
	and	eax, 0000ffffh
	mov	[JumpToRealMode + 1], ax

	;	Enter real mode 

	; 1. make sure interrupts are cleared

	cli

	; 2. Load 16 bit segment selectors

	mov eax, 20h
	mov ds, ax
	mov fs, ax
	mov gs, ax
	mov es, ax	
	mov ss ,ax

	; 3. Switch to real mode

	jmp 18h:RealModeSwitch

RealModeSwitch:		; we are now running in 16bit protected mode
			; (selector 18h) 

	BITS 16

	mov eax, cr0
	and al, ~1
	mov cr0, eax

JumpToRealMode:		; we are now running in real mode
	jmp	KERNEL_SEG:0a5a5h

	nop
	nop
	nop

	BITS 32

RealModeReturn:

	; reset data selector
	mov ax, 10h
	mov ds, ax
	mov es, ax

	; restore stack segment and stack pointer

	mov	ax, [ProtectedModeStackSegment]
	mov	ebx,[ProtectedModeStackPointer]

	mov	ss, ax
	mov	esp, ebx 	

	; Copy out registers to given structure

	mov	eax, [OutRegistersAddress]
	mov	edi, eax

	mov	esi, OutRegisters
	mov	ecx, (EndOutRegisters - OutRegisters) / 4

	rep	movsd

	; set return code

	mov	eax, [BIOSCarryFlag]

	; restore all registers

	pop	gs
	pop	fs
	pop	es
	pop	ds
	pop	esi
	pop	edi
	pop 	ecx
	pop    	ebx
  	pop    	ebp
  
	; need to check here if we need to reenable interrupts

	ret    

;====================== Real mode code for real mode wrapper	==============================

	BITS	16

RealModeEnter:

	; We are now in real mode
	; set up data segment from code segment

	mov ax, cs
	mov ds, ax

	; Load IDT

	lidt [realmode_idt_ptr - kernel_entry_point]

	; set up real mode stack

	mov ax,0000h
	mov ebx,7c00h
	mov ss, ax
	mov esp, ebx
	
	; set up registers

	mov ax, [InES - kernel_entry_point]
	mov es, ax

	mov ax, [InFS - kernel_entry_point]
	mov fs, ax

	mov ax, [InGS - kernel_entry_point]
	mov gs, ax

	mov eax, [InESI - kernel_entry_point]
	mov esi, eax

	mov eax, [InEDI - kernel_entry_point]
	mov edi, eax

	mov eax, [InEBP - kernel_entry_point]
	mov ebp, eax

	mov ebx, [InEBX - kernel_entry_point]
	mov ecx, [InECX - kernel_entry_point]
	mov edx, [InEDX - kernel_entry_point] 

	mov eax, [InEAX - kernel_entry_point] 

	; set data segment

	push ds
	push eax

	xor eax,eax
	mov ax, [InDS - kernel_entry_point]

	mov ds,ax

	pop eax
	
	; enable interrupts

	sti

	; execute interrupt

RealModeExecInt:
	int	55h

	; restore data segment

	pop ds

	; store eax

	mov [OutEAX - kernel_entry_point], eax

	; store carry

	jc Carry
	
	mov eax, 1
	mov [BIOSCarryFlag - kernel_entry_point], eax

Carry:
	
	; store rest of registers

	mov [OutEBX - kernel_entry_point], ebx
	mov [OutECX - kernel_entry_point], ecx
	mov [OutEDX - kernel_entry_point], edx

	mov eax, edi
	mov [OutEDI - kernel_entry_point], eax

	mov eax, esi
	mov [OutESI - kernel_entry_point], eax

	mov eax, ebp
	mov [OutEBP - kernel_entry_point], eax

	mov ax, fs
	mov [OutFS - kernel_entry_point], ax

	mov ax, gs
	mov [OutGS - kernel_entry_point], ax

	mov ax, es
	mov [OutES - kernel_entry_point], ax

	; Get ready to return to protected mode

	; Make sure interrupts are clear

	cli

	; Load idt and gdt
	
	lidt [kernel_idt_ptr - kernel_entry_point]
	lgdt [kernel_gdt_ptr - kernel_entry_point]

	; Set protected mode bit 

	mov eax, cr0
	or  al,1
	mov cr0, eax

	BITS 32

	; Clear prefetch queue

	; Load cs

	jmp ClearPrefetchQueue
	nop
	nop
ClearPrefetchQueue:

	; long jump to C program start
	
	db 066h		; for some reason, the disassembly is incorrect
	jmp 08h:LoadCS

LoadCS:
	
	mov eax, RealModeReturn

	jmp eax

	align 4

;========================= Interrupt jump table ===========================


	global boot_interruptJumpTable	
	extern io_handleInterrupt	

	align 32

%macro jmpTableEntry 2
	%if (%1 < 32 && ((0BFFD82FFh >> %1) & 1)) | (%1 >= 32) ; mask for interrupts without error code
		push byte 0	; push error code of zero
	%else
		nop		; pad out to 6 bytes
		nop
	%endif

	db	6ah		; push byte %1 (suppress warnings)
	db	%1 		; 6a xx
	
	jmp %2			; eb yy
%endmacro


%macro jmpTableSection 2
	%assign i %1
	%rep %2
		jmpTableEntry i, %%drain
	%assign i i+1
	%endrep
%%drain:
	jmp ISR
%endmacro

	align 8

boot_interruptJumpTable:

%assign j 0
%rep 12
	jmpTableSection j, 20
	align 8
%assign j j + 20
%endrep

	jmpTableSection j, 16	; 12 * 20 = 240 + 16 = 256

ISR:
	; preserve all registers

	cli

	push eax
	push ebx
	push ecx
	push edx

	push ds
	push es
	push fs
	push gs

	push edi
	push esi
	push ebp


	; Stack layout
	;	---------------- <- old ESP
	;	|   Old EFLAGS |   
	;	----------------
	;	|   Old CS     |   
	;	----------------
	;	|   Old IP     |   
	;	---------------- 
	;       |  Error code  |    
	;	---------------- < 30
	;	|  Interrupt   |    
	;	---------------- < 2c
	;	|  EAX         |    
	;	---------------- 
	;	|  EBX         |    
	;	----------------
	;	|  ECX         |    
	;	---------------- < 20
	;	|  EDX         |    
	;	----------------
	;	|  DS          |    
	;	----------------
	;	|  ES	       |    
	;	----------------
	;	|  FS          |    
	;	---------------- < 10
	;	|  GS          |    
	;	---------------- < c
	;	|  EDI         |    
	;	---------------- < 8
	;	|  ESI         |   
	;	---------------- <--- 4
	;	|  EBP         |    
	;	---------------- <--- ESP


	; set up stack frame for call to ISR

	mov eax, [esp + 02ch]	; interrupt
	mov ebx, [esp + 030h]   ; error code
	
	and eax, 000000ffh	; because two byte push sign extends, clear
				; extension.

	push eax	; first argument
	push ebx	; second argument
	
	cld

	; call ISR

	call	io_handleInterrupt

	;add esp, 8	; pop arguments from the stack
	
	pop ebx
	pop eax

	; restore registers

	pop ebp
	pop esi
	pop edi
	
	pop gs
	pop fs
	pop es
	pop ds

	pop edx
	pop ecx
	pop ebx
	pop eax

	add esp,8

	sti

	iret

;========================== Load IDT from C ==========================

	global	boot_loadIDT

boot_loadIDT:

	push ebp

	mov ebp, esp

	push eax

	mov eax, [ebp + 08h]

	mov [kernel_idt_ptr + 2], eax

	mov eax, 256*8	; IDT extent

	mov [kernel_idt_ptr], ax

	lidt [kernel_idt_ptr]

	pop eax
	pop ebp

	ret

	global boot_enableInterrupts

boot_enableInterrupts:

	sti

	ret

	global boot_disableInterrupts

boot_disableInterrupts:

	cli

	ret



