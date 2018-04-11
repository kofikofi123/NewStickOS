#ifndef _NSOACPI_H
#define _NSOACPI_H

#include "DataType.h"

struct RSDP {
    char Signature[8];
    u8 Checksum;
    char OEMID[6];
    u8 Revision;
    u32 RSDTAddress;
    u32 Length;
//May or may not be used vvvvvvvvvvvvvvv
    u64 XSDTAddress;
    u8 ExtendedChecksum;
    char Reserved[3];
};

struct ACPIHeader {
	char Signature[4];
	u32 Length;
	u8 Revision;
	u8 Checksum;
	char OEMID[6];
	char OEMTableID[8];
	u32 OEMRevision;
	u32 CreatorID;
	u32 CreatorRevision;

};

struct RSDT {
	struct ACPIHeader Header;
	u32* Entries;
};


struct XRSDT {
	struct ACPIHeader Header;
	u64* Entries;
};

void kernel_initACPI(void);
u8 kernel_isExtendedACPI(void);
void* kernel_getTableACPI(const char*);
struct RSDT* kernel_getRootTableACPI(void);




#endif