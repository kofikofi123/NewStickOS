global int_divide
global int_debug
global int_nmi
global int_breakpoint
global int_overflow

extern interrupt_handler

int_divide:
    push dword 0
    push dword 0
    pusha
    call interrupt_handler
    popa
    add esp, 8
    iret
    
int_debug:
    push dword 0 
    push dword 1
    pusha
    call interrupt_handler
    popa 
    add esp, 8
    iret
    
int_nmi:
    push dword 0
    push dword 2 
    pusha
    call interrupt_handler
    popa 
    add esp, 8
    iret
    
int_breakpoint:
    push dword 0 
    push dword 3
    pusha
    call interrupt_handler
    popa 
    add esp, 8
    iret
    
int_overflow:
    push dword 0 
    push dword 4
    pusha
    call interrupt_handler
    popa 
    add esp, 8
    iret