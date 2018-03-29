global kernel_storeGDT
global kernel_loadIDT
global kernel_cpuidCheck
global kernel_bcpuidnum
global kernel_wrmsr
global kernel_rdmsr
global kernel_enablePaging
global kernel_disablePaging
global kernel_loadPageDirectory

kernel_loadPageDirectory:
    push ebp
    mov ebp, esp
    push edx
    mov eax, dword [ebp + 8]
    mov edx, cr3
    or edx, eax 
    mov cr3, edx
    pop edx
    pop ebp
    ret
    
kernel_enablePaging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret
    
kernel_disablePaging:
    mov eax, cr0
    and eax, 0x7FFFFFFF
    mov cr0, eax
    ret
    
kernel_rdmsr: 
    push ebp 
    mov ebp, esp
    
    push ecx
    mov ecx, [ebp + 8]
    
    rdmsr

    pop ecx 
    pop ebp
    
    ret
kernel_wrmsr:
    push ebp
    
    mov ebp, esp 
    
    push eax 
    push edx
    push ecx
    
    xor eax, eax 
    xor edx, edx
    xor ecx, ecx
    
    
    mov ecx, [ebp + 8]
    mov eax, [ebp + 12]
    mov edx, [ebp + 16]
    
    wrmsr
    
    pop ecx 
    pop edx 
    pop eax
    
    pop ebp 
    
    ret
    
    
kernel_bcpuidnum:
    push ebp
    
    mov ebp, esp
    
    mov eax, 0
    cpuid
    
    pop ebp
    
    ret

kernel_storeGDT:
    push eax
    push ebp
    mov ebp, esp
    
    mov eax, [ebp + 12]
    sgdt [eax]
    
    pop ebp
    pop eax
    ret
    
kernel_loadIDT:
    push eax
    push ebp
    mov ebp, esp
    
    
    mov eax, [ebp + 12]
    lidt [eax]
    
    pop ebp
    pop eax 
    ret

;2/12/18
;2/20/18
;2/26/18