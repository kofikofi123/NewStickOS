#include "NSOACPI.h"
#include "NSOUtils.h"
#include "NSOBochs.h"

static const char* RSDP_SIGNATURE = "RSD PTR ";
static struct RSDP* root = NULL;

void kernel_initACPI(void){
    //check EBDA
    void* bda = (void*)(0x400);
    
    u32 addr = (u32)(*(u16*)(bda + 0x0E)) << 4;
    
    void* rootAddr = kernel_memscanString((void*)addr, RSDP_SIGNATURE, 0x400, 0x10);
    
    if (IS_NULL(rootAddr)){
        rootAddr = kernel_memscanString((void*)0x0E0000, RSDP_SIGNATURE, 0x20000, 0x10);
        if (IS_NULL(rootAddr)){
            return;
        }
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


    struct RSDT* rsdt = kernel_getRsdtACPI();

    char OEMID[5] = {0};

    kernel_memcpy(rsdt->Header.OEMID, OEMID, 4);
    kernel_printStringBOCHS(OEMID);
}

u8 kernel_isExtendedACPI(void){
    if (IS_NULL(root))
        return 0;
    
    return (root->Revision == 0) ? 0 : 1;
}

struct RSDT* kernel_getRsdtACPI(void){
    if (IS_NULL(root))
        return NULL;

    return (struct RSDT*)root->RSDTAddress;
}

void* kernel_getTableACPI(const char* tableName){
    struct RSDT* rsdt = kernel_getRsdtACPI();

    if (IS_NULL(rsdt))
        return NULL;

    return NULL; //for now
}