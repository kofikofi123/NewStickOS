#ifndef KERNEL_ACPI_H
#define KERNEL_ACPI_H

#include "DataType.h"

/*
struct kernel_ACPINamspace {
	
};*/

struct kernel_RSDP {
	char signature[8];
	u8 checksum;
	char oemid[6];
	u8 revision;
	u32 rsdtAddress;
	u32 length;
	u64 xsdtAddress;
	u32 extendedChecksum;
};

void kernel_initACPI();

#endif