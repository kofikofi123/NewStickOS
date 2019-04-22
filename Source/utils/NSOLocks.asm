section .text
	global kernel_acquireSpinlock
	global kernel_testSpinlock
	global kernel_releaseLock
	global kernel_acquireMutex


kernel_acquireSpinlock:
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]
.loop:
	lock bts dword [eax], 0
	jc .loop
.cleanup:
	mov esp, ebp
	pop ebp
	ret

kernel_testSpinlock:
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]
	mov eax, [eax]

	test eax, 1

	mov esp, ebp
	pop ebp
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
	push ebp
	mov ebp, esp
	push ecx

	mov eax, [ebp + 8]
	xor ebx, ebx
	mov ecx, 1

	lock cmpxchg eax, ecx
	test ecx, 1

	

	pop ebx
	mov esp, ebp	
	pop ebp
	ret