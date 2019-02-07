#ifndef KERNEL_PFALLOCATOR_H
#define KERNEL_PFALLOCATOR_H

#define KERNEL_PAGE_CACHE_LIMIT 50

u8 kernel_initPageAllocator();
void* kernel_allocatePages();
void kernel_freePages(void*);

#endif