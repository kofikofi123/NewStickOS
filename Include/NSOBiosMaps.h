#ifndef KERNEL_BIOSMEMMAP_H
#define KERNEL_BIOSMEMMAP_H

#include "DataType.h"

struct kernel_BMM {
	u64 base;
	u64 len;
	u32 type;
	u32 acpi;
};

void kernel_initMemMapB();
void kernel_debugMemMap();
u32 kernel_getLargestAddr();
u32 kernel_getEntryLength();
struct kernel_BMM* kernel_getEntry(u32);
struct kernel_BMM* kernel_lookupMemMapB(void*);
struct kernel_BMM* kernel_lookMemMapSB(u32, u32);

#endif