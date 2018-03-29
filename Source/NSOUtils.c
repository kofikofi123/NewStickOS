#include "NSOUtils.h"

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