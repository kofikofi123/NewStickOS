#ifndef NSO_MEMALLOC_H
#define NSO_MEMALLOC_H

#include "DataType.h"

struct MemoryBufferPointer {
    void* base;
    u32 length;
    
    void* next;
    void* prev;
};

void kernel_initMemoryAllocator(void);
void* kernel_malloc(u32);
void* kernel_mallocAligned(u32, u16);
void kernel_free(void*);


#endif