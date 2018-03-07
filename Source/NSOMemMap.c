#include "MemMap.h"

static void* memmap_base = (void*)(0x7E00 - 0x2000);

struct BIOSMemoryMap* get_entry(u16 offset){
    u16 moff = offset * 0x18;
    return (memmap_base + moff);
}