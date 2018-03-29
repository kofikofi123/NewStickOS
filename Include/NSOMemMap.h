#ifndef K_MEMMAP_H
#define K_MEMMAP_H 1 

#include "DataType.h"

struct BIOSMemoryMap {
    u64 base;
    u64 limit; 
    u32 type;
    u32 acpi;
};

void kernel_initMemoryMap(void);
struct BIOSMemoryMap* kernel_getMemEntry(u16);
struct BIOSMemoryMap* kernel_getMemEntryViaAddress(u32);
u8 kernel_addressInMemRange(struct BIOSMemoryMap*, u32);


#endif 