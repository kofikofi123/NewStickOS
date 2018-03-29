#ifndef _UTILS_H
#define _UTILS_H

#ifndef NULL
#define NULL (void*)0
#endif

#define IS_NULL(x) (x == NULL)

#include "DataType.h"

void kernel_halt(void);
void kernel_cli(void);
void kernel_warn(const char*);
//possible remove for bottom
void kernel_memcpy(const void*, void*, u32);
void kernel_memset(const void*, u8, u32);

#endif