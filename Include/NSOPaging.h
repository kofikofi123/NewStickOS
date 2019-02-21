#ifndef KERNEL_PAGING
#define KERNEL_PAGING

#include "DataType.h" 

void kernel_initPaging();

u8 kernel_mapAddress(u32, u32);
u8 kernel_mapIdentity(u32, u16);
void kernel_releasePage(void*);
void kernel_loadPageDirectory(void*);
u32* kernel_loadPageTable(u16);
void kernel_enablePaging();
void kernel_updatePaging();


#endif