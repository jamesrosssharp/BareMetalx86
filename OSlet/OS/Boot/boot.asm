
	bits 32

	extern main
	global start

	global boot_realModeIntWrapper

	; important constants

KERNEL_SEG	equ	02000h
KERNEL_OFFSET	equ	KERNEL_SEG*10h



;======================== Entry from Bootloader	==============================

start:

	; We don't trust the bootloader's idt and gdt.
	; Load our own here.




	; Long jump to set cs segment
	



	; rest of init

	mov eax, 00000010h
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax
	mov ss, ax
	mov esp, 0008fff0h	; Move the stack well away from the EBDA typical location

	finit

	jmp	main


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
RealModeInterruptToCall:
		dd	0
BIOSCarryFlag:
		dd	0
ProtectedModeStackPointer:
		dd	0
ProtectedModeStackSegment:
		dd	0
RealModeReturnAddress:
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
 
  	;mov    eax,DWORD PTR [ebp+0x8]	;	Interrupt to call 
  	;mov    ebx,DWORD PTR [ebp+0xc]	;	In registers
  	;mov    ecx,DWORD PTR [ebp+0x10]	;	Out registers
 	
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
	mov	[RealModeInterruptToCall], eax

	;	Preserve stack pointer and stack segment

	mov	ax,ss
	mov	[ProtectedModeStackSegment], ax

	mov	eax, esp
	mov	[ProtectedModeStackPointer], eax

	;	Set the return address from real mode

	mov	eax,RealModeReturn
	mov	[RealModeReturnAddress], eax 

	; self-modify the address to jump to

	mov	eax, RealModeEnter
	and	eax, 0000ffffh
	mov	[JumpToRealMode + 1], ax

	;	Enter real mode 

	; 1. make sure interrupts are cleared

	cli

	; 2. Load 16 bit segment selectors


	; 3. Switch to real mode

	; 4. Set valid data segment


	; 5. Load IVT	
		
	; 6. Jump to real mode code

	jmp	RealModeReturn

	BITS 16

JumpToRealMode:
	jmp	KERNEL_SEG:0a5a5h

	nop
	nop
	nop

	BITS 32

RealModeReturn:

	; restore stack segment and stack pointer

	mov	ax, [ProtectedModeStackSegment]
	mov	ebx,[ProtectedModeStackPointer]

	mov	ss, ax
	mov	esp, ebx 	

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
  	ret    

;====================== Real mode code for real mode wrapper	==============================

	BITS	16

RealModeEnter:

	; We are now in real mode
	; set up data segment from code segment


	; set up real mode stack


	; ds:bp now points to the real mode wrapper data structure.
	
	; set up registers


	; Get ready to return to protected mode



	; Load idt and gdt

	; Make sure interrupts are clear

	cli

	; Set protected mode bit 


	; Clear prefetch queue


	; Load return address


	; Jump back to protected mode
	align 4
