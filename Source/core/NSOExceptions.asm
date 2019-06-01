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
	pop ax
	pop ss
	pop ax
	pop ds
	popad
%endmacro

	
section .text
	global kernel_isr0
	global kernel_isr8
	global kernel_isr13
	global kernel_isr14

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
	