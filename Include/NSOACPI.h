#ifndef _NSOACPI_H
#define _NSOACPI_H

#include "DataType.h"

struct RootDirectorySystemPointer {
    char Signature[8];
    u8 Checksum;
    char OEMID[6];
    u8 Revision;
    u32 RSDTAddress;
    u32 Length;
    u64 XSDTAddress;
    u8 ExtendedChecksum;
    char Reserved[3];
};

void kernel_initACPI(void);
u8 kernel_isExtendedACPI(void);


#endif