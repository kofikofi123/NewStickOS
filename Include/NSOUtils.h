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

void* kernel_memscanString(const void*, const void*, u32, u8);
u64 kernel_checksum(void*, u32);


void kernel_outB(u16, u8);
void kernel_outW(u16, u16);
void kernel_outDW(u16, u32);
u8 kernel_inB(u16);
u16 kernel_inW(u16);
u32 kernel_inDW(u16);

#endif