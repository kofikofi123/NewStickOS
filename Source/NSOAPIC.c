#include "NSOAPIC.h"
#include "NSOCpuID.h"
#include "NSOMsr.h"
#include "NSOUtils.h"

static struct CpuIDRegs _regs_;

u8 kernel_checkAPIC(void){
    if (!kernel_checkCPUID(0x01)){
        
        return 0;
    }
    kernel_cpuid(0x01, &_regs_);
    
    return ((_regs_.edx >> 8) & 0x01);
}

void kernel_enableAPIC(void){
    kernel_wrmsr(0x1B, (kernel_rdmsr(0x1B)) | 0x100);
}

void kernel_disableAPIC(void){
    kernel_wrmsr(0x1B, (kernel_rdmsr(0x1B) & 0xFFFFFEFF));
}

void* kernel_getAPICAddress(void){
    if (!kernel_checkAPIC()){
        return NULL;
    }
    
    return (void*)(u32)(((kernel_rdmsr(0x1B) >> 12) & 0xFFFFFF) << 12);
}