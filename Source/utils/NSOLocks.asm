section .text
	global kernel_acquireSpinlock
	global kernel_testSpinlock
	global kernel_releaseLock
	global kernel_acquireMutex


kernel_acquireSpinlock:
	;cli
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]
.loop:
	lock bts dword [eax], 0
	jc .loop
.cleanup:
	mov esp, ebp
	pop ebp
	;sti
	ret

kernel_releaseLock:
	push ebp 
	mov ebp, esp
	mov eax, [ebp + 8]

	lock btr dword [eax], 0

	mov esp, ebp
	pop ebp
	ret

kernel_acquireMutex:
	;cli
	push ebp
	mov ebp, esp

	push edi
	push ecx


	mov ecx, 1
	mov edi, [ebp + 8]
	mov eax, 0

	lock cmpxchg [edi], ecx

	xor eax, 1
	
	pop ecx
	pop edi
	
	mov esp, ebp	
	pop ebp
	;sti
	ret