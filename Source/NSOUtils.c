#include "NSOUtils.h"

inline void kernel_halt(void){
    __asm__("hlt");
}

inline void kernel_cli(void){
    __asm__("cli");
}