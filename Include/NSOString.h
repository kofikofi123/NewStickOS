#ifndef _NSOSTRING_H
#define _NSOSTRING_H

#include "DataType.h"

u32 kernel_stringLength(const char*);
u8 kernel_stringCompare(const char*, const char*);
u8 kernel_stringCompareRaw(const char*, const char*, u32);

#endif