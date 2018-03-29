#include "NSOMemMap.h"
#include "NSOUtils.h"

static void* mem_settings;
static void* memmap_base;

void kernel_initMemoryMap(void){
    mem_settings = (void*)0x500;
    memmap_base = (void*)*(u32*)(mem_settings + 0x18);
}

struct BIOSMemoryMap* kernel_getMemEntry(u16 offset){
    u16 moff = offset * 0x18;
    return (struct BIOSMemoryMap*)(memmap_base + moff);
}

struct BIOSMemoryMap* kernel_getMemEntryViaAddress(u32 address){
    u32 entries = *(u32*)mem_settings;
    
    struct BIOSMemoryMap* temp;
    
    for (u32 i = 0; i < entries; i++){
        temp = kernel_getMemEntry(i);
        
        if (kernel_addressInMemRange(temp, address))
            return temp;
    }
    
    return NULL;
}

u8 kernel_addressInMemRange(struct BIOSMemoryMap* mem_map, u32 addr){
    if (mem_map->base <= addr && mem_map->limit > addr){
        return 1;
    }
    return 0;
}