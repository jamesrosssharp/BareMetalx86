	
	bits 32

	extern main
	global start

start:

	mov eax, 00000010h
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax
	mov ss, ax
	mov esp, 0008fff0h	; Move the stack well away from the EBDA typical location

	finit

	jmp	main


