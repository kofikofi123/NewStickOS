#ifndef _NSOCPUID_H
#define _NSOCPUID_H

#include "DataType.h"

struct CpuIDRegs {
    u32 eax, ebx, ecx, edx;
};

u8 kernel_checkCPUID(u32);
void kernel_cpuid(u32, struct CpuIDRegs*);

#endif