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
}__attribute__((packed)); //just to  be sure

struct kernel_ACPIHeader {
	char signature[4];
	u32 length;
	u8 revision;
	u8 checksum;
	char oemid[6];
	char oemTableID[8];
	u32 oemRevision;
	u32 creatorID;
	u32 creatorRevision;
}__attribute__((packed)); //just to  be sure

struct kernel_ACPIObject {
	const char* name;
};

struct kernel_ACPINamespace {
	const char* name;
};

u8 kernel_initACPI();
void* kernel_findACPITable(const char*);
u8 kernel_loadAML(struct kernel_ACPINamespace*, void*);

#endif