	bits 32

	extern main
	global start

	section .startup

start:

	; load GDT and IDT


	; load new CS selector


	; init all registers

	mov eax, 00000010h
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax

	; set up a virtual stack	

	mov ss, ax
	mov esp, 0008fff0h

	; init FPU
	
	finit			; Should really do this once we have detected
				; that the CPU has an FPU

	cld			; clear direction flag

	; prepare initial paging tables


	; enable paging


	; jump into virtual memory

	jmp	main


