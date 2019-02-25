#ifndef KERNEL_PFALLOCATOR_H
#define KERNEL_PFALLOCATOR_H

#define KERNEL_PAGE_CACHE_LIMIT 64

void kernel_initPageAllocator();
void* kernel_allocatePage();
void kernel_markPage(u32);
void kernel_markPageAddr(void*);
void kernel_freePage(void*);
u32 kernel_findPages(u32);

#endif