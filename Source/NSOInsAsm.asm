global kernel_storeGDT
global kernel_loadIDT


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