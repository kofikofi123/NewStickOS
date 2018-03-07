#ifndef K_MEMMAP_H
#define K_MEMMAP_H 1 

#include "DataType.h"

struct BIOSMemoryMap {
    u64 base;
    u64 limit; 
    u32 type;
    u32 acpi;
};


struct BIOSMemoryMap* get_entry(u16);


#endif 