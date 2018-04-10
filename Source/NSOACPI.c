#include "NSOACPI.h"
#include "NSOUtils.h"

static const char* RSDP_SIGNATURE = "RSD PTR ";
static struct RSDP* root;

void kernel_initACPI(void){
    //check EBDA
    void* bda = (void*)(0x400);
    
    u32 addr = (u32)(*(u16*)(bda + 0x0E)) << 4;
    void* phyAddr = (void*)addr;
    
    void* rootAddr = kernel_memscanString(phyAddr, RSDP_SIGNATURE, 0x400, 0x10);
    
    if (rootAddr == NULL){
        phyAddr = (void*)0x0E0000;
        rootAddr = kernel_memscanString(phyAddr, RSDP_SIGNATURE, 0x20000, 0x10);
    }
    
    if (rootAddr == NULL){
        return;
    }
    
    root = rootAddr;
    
    
    u64 test = kernel_checksum(root, 20);
    
    __asm__("mov eax, %0"
            :
            : "r" (test));
    
    kernel_halt();
}