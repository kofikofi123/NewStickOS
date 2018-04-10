#ifndef NSO_PAGING_H
#define NSO_PAGING_H

#include "DataType.h"

/*
struct PageDirectory {
    u8 present : 1;
    u8 readWrite : 1;
    u8 userModeAccess : 1;
    u8 pageLevelWriteThrough : 1;
    u8 pageLevelCacheDisable : 1;
    u8 accessed : 1;
    u8 padding : 1;
    u8 size : 1;
    u8 info : 4;
    u32 pageAddress : 20;
};*/
/*
struct PageTable {
    u8 present : 1;
    u8 readWrite : 1;
    u8 userModeAccess : 1;
    u8 pageLevelWriteThrough : 1;
    u8 pageLevelCacheDisable : 1;
    u8 accessed : 1;
    u8 dirty : 1;
    u8 pat : 1;
    u8 global : 1;
    u8 info : 4;
    u32 physicalAddress : 20;
};*/

void* kernel_allocPage(u32);
void kernel_freePage(void*);
void kernel_enablePaging(void);
void kernel_disablePaging(void);
void kernel_initPaging(void);
void kernel_updatePaging(void);
void kernel_mapPage(u32, u32);
void* kernel_findPhysicalAddress(u32);
void kernel_loadPageDirectory(void*);


#endif