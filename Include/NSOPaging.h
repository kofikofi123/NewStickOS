#ifndef KERNEL_PAGING
#define KERNEL_PAGING

#include "DataType.h" 

void kernel_initPaging();

u8 kernel_reservePage(void*);
u8 kernel_reservePage2(void*, void*);
u8 kernel_releasePage(void*);


#endif