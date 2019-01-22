BITS 16 

ORG 0x7E00

jmp 0x00:entry


Print_string: ;we are going to use the interrupt service because I do not need to directly write to it right now
    pusha
    pushf 
    cld
    
    mov ah, 0x0E
.looper:
    lodsb
    
    or al, al 
    jz .fini
    
    int 0x10 
    jmp .looper
.fini:
    popf
    popa 
    ret
    
check_a20_enable:
    ;going to use osdev address of checking, because the value should already be there
    ;check bootload signature to see if it is the same (when read from 1 MB above) aka if wraparound exist
    pusha ;lazy method
    push ds 
    push fs 
    
    mov ax, 0x0000
    mov ds, ax 
    mov ax, 0xFFFF 
    mov fs, ax 
    
    mov ax, word [ds:0x7DFE]
    mov bx, word [fs:0x7E0E]
    
    
    xor ax, bx 
    
    
    pop fs 
    pop ds 
    popa
    ret

delay_ps2:
    push ax 
.tester:
    in ax, 0x64
    test ax, bx 
    jz .tester
.done:
    pop ax
    ret

keyboard_a20:
    push ax 
    push bx 

    mov ax, 0xD0
    out 0x64, ax
    mov bx, 0x02
    call delay_ps2
    in ax, 0x60
    or ax, 0x02 
    push ax 
    mov ax, 0xD1 
    out 0x64, ax 
    pop ax
    mov bx, 0x01
    call delay_ps2
    out 0x60, ax 
    
    pop bx 
    pop ax 
    ret 
    
bios_a20:
    push ax 
    
    mov ax, 0x2401
    int 0x15 
    
    pop ax 
    ret 
entry:
    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    
    cli 
    mov ss, ax 
    mov sp, 0x7E00-0x2000-1
    sti 
    
    call check_a20_enable
    jnz short .setup_memory_table 
    
    call keyboard_a20
    call check_a20_enable 
    jnz short .setup_memory_table 
    
    call bios_a20
    call check_a20_enable
    jnz short .setup_memory_table
    
    lea si, [error_message1]
    call Print_string
    jmp .jumper
.setup_memory_table:
    mov eax, 0xE820
    mov ebx, 0x0000
    mov edi, 0x530
    mov ecx, 0x18
    mov edx, 0x534D4150
    
    mov dword [edi + 20], 1
    
    int 0x15
    jmp .check_memory_table
.loop_memory_table: ;repetitive
    mov eax, 0xE820
    mov ebx, dword [mq_code]
    mov ecx, 0x18 
    mov edx, 0x534D4150
    mov dword [edi + 20], 1
    
    int 0x15
.check_memory_table:
    jc short .error_memory_table
    cmp eax, 0x534D4150
    jne short .error_memory_table
    cmp ecx, 20
    je .mem_addr
.check_acpi_extended:
    cmp dword [edi + 20], 1
    je .loop_memory_table
.mem_addr:
    or ebx, ebx 
    jz short .finish_up
    mov dword [mq_code], ebx
    add edi, 0x18
    inc dword [mem_entry_count]
    jmp short .loop_memory_table
.finish_up:
    mov eax, dword [mem_entry_count]
    mov dword [0x500], eax
    mov dword [0x518], 0x530 ;added this incase I decide to move memory map somewhere, and don't feel like replacing everything
.load_gdt:
    lgdt [temp_gdtr]
    cli
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax
    jmp 0x08:protected_world
.error_memory_table:
    lea si, [error_message2]
    call Print_string
.jumper:
    cli 
    hlt 
    jmp short .jumper
    
ALIGN 8 

temp_gdt:
    ;null descriptor 
    dq 0
    ;code descriptor
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x99
    db 0xDF
    db 0x00
    ;data descriptor
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x93
    db 0xDF
    db 0x00
temp_gdt_end:


temp_gdtr:
    dw temp_gdt_end - temp_gdt - 1 
    dd temp_gdt 



mq_code: dd 0 
error_message1: db "Could not enable a20 bit", 0
error_message2: db "Could not get memory map", 0
test_str: db "eok", 0
mem_entry_count: dd 0

BITS 32 
ALIGN 4
protected_world:
    mov ax, 0x10
    mov ds, ax 
    mov fs, ax 
    mov gs, ax 
    mov es, ax
    ;interrupt flag is already cleared on entry
    mov ss, ax 
    mov sp, 0x7E00-0x2000-1
    
    jmp 0x08:0x10000
