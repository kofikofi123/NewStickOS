#ifndef _ISO9660_H
#define _ISO9660_H

#include "DataType.h"
//everything in this file is not final
typedef struct {
    const char* filename;
    u32 lba_address;
    uint8_t entity_flag;
} Entity;

#endif