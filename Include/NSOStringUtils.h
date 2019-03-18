#ifndef KERNEL_STRUTILS_H
#define KERNEL_STRUTILS_H 

#include "DataType.h"

u32 kernel_stringLength(const char*);
u32 kernel_stringOccurance(const char*, const char);

u8 kernel_stringCompareRAW(const char*, const char*, u32);

#endif