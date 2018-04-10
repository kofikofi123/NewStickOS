#include "NSOACPI.h"
#include "NSOUtils.h"

static const char* RSDP_SIGNATURE = "RSD PTR ";
static struct RootDirectorySystemPointer* root = NULL;

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
    
    
    u64 checksum = kernel_checksum(root, 20);
    
    if ((checksum & 0xFF) != 0)
        return;
    
    if (kernel_isExtendedACPI()){
        checksum = kernel_checksum(root, root->Length);
        
        if ((checksum & 0xFF) != 0)
            return;
    }
}

u8 kernel_isExtendedACPI(void){
    if (root == NULL)
        return 0;
    
    return (root->Revision == 0) ? 0 : 1;
}