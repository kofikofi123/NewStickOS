#ifndef KERNEL_ACPI_H
#define KERNEL_ACPI_H

#include "DataType.h"

//#define KERNEL_AML_Namespace 0 //not technically an object
#define KERNEL_AML_UNINITALIZED 0
#define KERNEL_AML_INTEGER 1
#define KERNEL_AML_STRING 2
#define KERNEL_AML_BUFFER 3
#define KERNEL_AML_PACKAGE 4
#define KERNEL_AML_FIELDUNIT 5
#define KERNEL_AML_DEVICE 6
#define KERNEL_AML_EVENT 7
#define KERNEL_AML_METHOD 8
#define KERNEL_AML_MUTEX 9
#define KERNEL_AML_OPERATIONREGION 10
#define KERNEL_AML_POWERRESOURCE 11
#define KERNEL_AML_PROCESSOR 12
#define KERNEL_AML_THERMALZONE 13
#define KERNEL_AML_BUFFERFIELD 14
#define KERNEL_AML_DDBHANDLE 15
#define KERNEL_AML_DEBUGOBJECT 16



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

struct kernel_ACPIScope {
	u32 type;
	char name[4];
	struct kernel_ACPIScope* parentScope;
	struct kernel_ACPIScope* nextScope;
	struct kernel_ACPIScope* childScope;
	struct kernel_ACPIHeader* SDT;
	u8 isReference;
	union {
		u64 integer;
		const char* string;
		struct kernel_ACPIScope* reference;
	}data;
};

u8 kernel_initACPI();
void* kernel_findACPITable(const char*);
u8 kernel_loadAML(struct kernel_ACPIScope*, void*);
void kernel_debugACPITree(struct kernel_ACPIScope*);
void kernel_preloadACPIRoot(struct kernel_ACPIScope*);
u32 kernel_getACPIRevision();
struct kernel_ACPIScope* kernel_searchScope(struct kernel_ACPIScope*, const char*); 
#endif
