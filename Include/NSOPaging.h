#ifndef KERNEL_PAGING
#define KERNEL_PAGING

#include "DataType.h" 

void kernel_initPaging();

u8 kernel_mapAddress(u32, u32, u8);
u8 kernel_mapIdentity(u32, u32, u8);

u32* kernel_createVirtualAddressSpace();

void kernel_unmapAddress(u32);
void kernel_unmapIdentity(u32, u32);

void* kernel_getPhysicalAddress(u32);

void* kernel_vAllocatePage(u32, u32, u8);
void kernel_vFreePage(u32, u32);

void kernel_enablePaging();
void kernel_updatePaging();
void kernel_invalidatePaging(void*);
void kernel_loadPageDirectory(void*);


#endif