section .text
	global kernel_loadPageDirectory
	global kernel_enablePaging
	global kernel_updatePaging

kernel_loadPageDirectory:
	push ebp
	mov ebp, esp

	push ebx
	push eax

	mov ebx, [ebp + 8]
	mov eax, cr3
	or eax, ebx
	mov cr3, eax

	pop eax
	pop ebx

	mov esp, ebp
	pop ebp
	ret

kernel_enablePaging:
	push eax
	mov eax, cr0
	or eax, 0x80000000
	xchg bx, bx
	mov cr0, eax
	pop eax
	ret

kernel_updatePaging:
	mov eax, cr3
	mov cr3, eax
	ret