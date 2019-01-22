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
struct kernel_BMM* kernel_lookupMemMapB(void*);

#endif