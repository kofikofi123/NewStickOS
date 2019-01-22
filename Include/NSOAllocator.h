#ifndef KERNEL_ALLOCATOR_H
#define KERNEL_ALLOCATOR_H

#include "DataType.h"

void kernel_init_allocation();

void* kernel_malloc(u32);
void kernel_free(void*);
void kernel_alloc_debug();

#endif