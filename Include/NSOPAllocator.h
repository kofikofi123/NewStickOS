#ifndef KERNEL_PFALLOCATOR_H
#define KERNEL_PFALLOCATOR_H

#define KERNEL_PAGE_CACHE_LIMIT 50

void kernel_initPageAllocator();
void* kernel_allocatePage();
void kernel_freePage(void*);

#endif