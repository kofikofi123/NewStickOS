global int_divide
global int_debug
global int_nmi
global int_breakpoint
global int_overflow

extern interrupt_handler

int_divide:
    push dword 0
    push dword 0
    cli 
    pusha
    call interrupt_handler
    popa
    add esp, 8
    iret
    
int_debug:
    push dword 0 
    push dword 1 
    cli
    pusha
    call interrupt_handler
    popa 
    add esp, 8
    iret
    
int_nmi:
    push dword 0
    push dword 2 
    cli 
    pusha
    call interrupt_handler
    popa 
    add esp, 8
    iret
    
int_breakpoint:
    push dword 0 
    push dword 3
    cli
    pusha
    call interrupt_handler
    popa 
    add esp, 8
    iret
    
int_overflow:
    push dword 0 
    push dword 4
    cli
    pusha
    call interrupt_handler
    popa 
    add esp, 8
    iret