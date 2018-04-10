#include "NSOUtils.h"
#include "NSOString.h"

inline void kernel_halt(void){
    __asm__("hlt");
}

inline void kernel_cli(void){
    __asm__("cli");
}

void kernel_memcpy(const void* source, void* destination, u32 iterations){
    char* b_source = (char*)source;
    char* b_dest = (char*)destination;
    char* b_end = (b_source + iterations - 1);
    
    
    while (b_source != b_end){
        *b_dest++ = *b_source++; 
    }
    
}
void kernel_memset(const void* destination, u8 value, u32 iterations){
    char* b_dest = (char*)destination;
    char* b_end = (b_dest + iterations - 1);
    
    while (b_dest != b_end){
        *b_dest++ = value;
    }
}

void* kernel_memscanString(const void* source, const void* source2, u32 length, u8 alignment){
    char* tempA = (char*)source;
    char* tempB = (char*)source2;
    u32 tempB_length = kernel_stringLength(source2);
    char* tempEnd = tempA + length - 1;
    if (((u32)tempA % alignment) != 0)
        return NULL;
        
    while (tempA <= tempEnd){
        if (kernel_stringCompareRaw(tempA, tempB, tempB_length)){
            return (void*)tempA;
        }
        
        tempA += alignment;
    }
    
    return NULL;
}

u64 kernel_checksum(void* addr, u32 length){
    u64 temp = 0;
    u8* tempAddr = (u8*)addr;
    u8* tempAddrEnd = tempAddr + length - 1;
    
    while (tempAddr != tempAddrEnd){
        temp = temp + (*tempAddr++);  
    }
    
    return temp;
}