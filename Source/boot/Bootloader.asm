;Do not test anything below, please
;Warning if you do run it: I did not finish some of the operation, or even test it.
;This means it could get at places, mess something up, error
BITS 16

ORG 0x7C00

jmp 0x00:Main

Reset_drive:
    pusha
    mov ah, 0x00
    int 0x13
    popa 
    ret
    
String_length:
    pushf
    push cx 
    mov cx, 0
    cld
.loper:
    lodsb 
    
    or al, al
    jz .done
    
    inc cx 
    jmp .loper
.done:
    xchg ax, cx 
    pop cx 
    popf 
    ret 

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
    
Get_Disk_Extention:
    pusha
    
    mov ax, 0x41
    mov bx, 0xAA55
    mov dl, [bdrive]
    int 0x13
.exit:    
    popa
    ret 

Read_sector:;register destroyer
    push cx 
    push bx
    push di
    push si
    mov ecx, 3
    mov word [disk_packet_struct.sectors_to_read], ax 
    mov word [disk_packet_struct.read_address], di
    mov word [disk_packet_struct.read_address + 2], bx 
    mov word [disk_packet_struct.lba_address], si 


.begin:
    mov dl, [bdrive]
    lea si, [disk_packet_struct] ;ds should already be 0
    mov ah, 0x42
    pusha
    int 0x13
    popa
    jnc .noset
    loop .begin
    xor ax, ax 
    jmp .fini 
.noset:
    mov ax, 1
.fini:
    pop si
    pop di
    pop bx
    pop cx
    ret


Validate_volume:
    pusha
    
    lea si, [filesystel_validation_string]
    lea di, [disk_buffer + 1]
    mov cx, 5
    
    repe cmpsb
    
    popa
    ret 


Validate_volume_id:
    call Validate_volume
    jne .exit 
    
    push bx 
    mov bl, byte [disk_buffer]
    cmp al, bl 
    pop bx
.exit:
    ret
    
Read_file:
    pusha
    push cx 
    push di
    jmp .search_file 
.looper:
    pop si
    movzx ax, byte [bx]
    add bx, ax
.search_file:
    push si
    movzx cx, [bx + 32]
    lea di, [bx + 33]
    
    repe cmpsb 
    jne .looper 
.calculate_number_of_sectors:
    pop si
    pop di
    mov ax, word [bx + 10] ;poor
    mov dx, word [bx + 12]
    mov cx, 0x800
    div cx


    
    or dx, dx 
    jz .read_the_file
    inc ax  
.read_the_file:
    mov si, word [bx + 2]
    pop bx
    
    call Read_sector 
.exit:
    popa
    ret

Main:
    xor ax, ax 
    mov ds, ax
    mov es, ax 

    cli
    mov ss, ax
    mov sp, 0x7BFF
    sti 
    
    mov [bdrive], dl
    

    call Reset_drive 
.check_exten:
    call Get_Disk_Extention
    jnc .setup_read_primary
    lea si, [error_message2]
    jmp .errorout
.setup_read_primary:
    mov ax, 1
    mov si, 0x10
    lea di, [disk_buffer]
    mov bx, 0 
    mov cx, 5
    jmp .repeat_read_until_primary
.rep_counter:
    inc si
.repeat_read_until_primary:
    push cx
    call Read_sector
    pop cx
    or ax, ax
    jnz .primary_read ;could have used loop ins
    loop .repeat_read_until_primary
    lea si, [error_message4]
    jmp .errorout
.primary_read:
    push ax
    
    mov al, 0x01
    call Validate_volume_id 
    je .setup_read_files 
    mov al, 0xFF
    call Validate_volume_id
    pop ax 
    
    jne .rep_counter 
    lea si, [error_message6]
    jmp .errorout 
.setup_read_files:
    mov esi, dword [disk_buffer + 158]
    lea di, [disk_buffer]
    mov ax, 1 
    xor bx, bx
    call Read_sector
    
    or ax, ax
    jnz .read_files
    
    lea si, [error_message4] ;lazy
    jmp .errorout
.read_files:
    lea si, [kernel_file_name]
    lea bx, [disk_buffer]
    xor di, di 
    mov cx, 0x1000
    
    call Read_file
    
    push ax
    
    lea si, [boot_setting_file_name]
    lea bx, [disk_buffer]
    mov cx, 0
    mov di, bx
    
    
    call Read_file 
    
    pop bx 
    
    and ax, bx 
    
    ;or ax, ax 
    
    
    jnz short .jump_to_setup
    lea si, [error_message5]
    jmp short .errorout
.jump_to_setup:
    jmp disk_buffer
.errorout:
    pusha
    call Print_string
    popa
.loper:
    cli
    hlt
    jmp .loper
;variables
bdrive: db 0

error_message2: db "BIOS extention not supported", 0
error_message4: db "Unknown error", 0
error_message5: db "Missing files", 0
error_message6: db "Could not find primary descriptor", 0

kernel_file_name db "STICK.BIN;1"
boot_setting_file_name db "SETUP.BIN;1"
filesystel_validation_string db "CD001"

disk_packet_struct:
    db 0x10
    db 0 
.sectors_to_read:
    dw 0
.read_address:
    dw 0
    dw 0
.lba_address:
    dq 0

times 510-($-$$) db 0
db 0x55
db 0xAA

section .bss
disk_buffer: resb 2048
