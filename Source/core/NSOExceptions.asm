%macro isr_entry 0
	pushad
	xor eax, eax
	mov eax, ds
	push eax
	mov eax, ss
	push eax
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
%endmacro
%macro isr_clean 0
	pop eax
	mov ss, eax
	pop eax
	mov ds, eax
	popad
%endmacro

	
section .text
	global kernel_isr0
	global kernel_isr8
	global kernel_isr13
	global kernel_isr14
	global kernel_isr32
	global kernel_isrstub_start
	global kernel_isrstub_end
	
kernel_isr0:
	extern kernel_divEX
	push dword 0
	isr_entry
	call kernel_divEX
	isr_clean
	add esp, 4
	iret

kernel_isr8:
	extern kernel_doubleFaultEX
	isr_entry
	call kernel_doubleFaultEX
	isr_clean
	add esp, 4
	iret

kernel_isr13:
	extern kernel_generalProtectionEX
	isr_entry
	call kernel_generalProtectionEX
	isr_clean
	add esp, 4
	iret

kernel_isr14:
	extern kernel_pagingEX
	isr_entry
	call kernel_pagingEX
	isr_clean
	add esp, 4
	iret

;I am going to use this neat trick linux does SOURCE: entry_32.s

ALIGN 4
kernel_isrstub_start:
	%assign vector 0x20
	%rep 0xC0-0x20
		push strict dword vector
		jmp kernel_isrentry
		ALIGN 4
	%assign vector vector + 1
	%endrep
kernel_isrstub_end: times 128 db 0

kernel_isrentry:
	extern kernel_isrHandler
	isr_entry
	call kernel_isrHandler
	isr_clean
	add esp, 4
	iret