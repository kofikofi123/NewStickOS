#include "NSOCpuID.h"

u32 kernel_bcpuidnum(void);

u8 kernel_checkCPUID(u32 check){
    u32 eax = kernel_bcpuidnum();
    
    
    return (check <= eax);
}

void kernel_cpuid(u32 vendor, struct CpuIDRegs* regs){
    __asm__("cpuid"
            : "=a" (regs->eax), "=b" (regs->ebx), "=c" (regs->ecx), "=d" (regs->edx)
            : "a" (vendor));
}