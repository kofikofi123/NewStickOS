#ifndef _NSO_DESCRIPTORS
#define _NSO_DESCRIPTORS 0

#include "DataType.h"

struct SegmentDescriptor {
      u16 Limit0;
      u16 Base0;
      u8 Base1;
      u8 Type0;
      u8 Type1;
      u8 Base2;
}__attribute__((packed));

struct InterruptDescriptor {
    u16 Offset0;
    u16 Selector;
    u8 Type0;
    u8 Type1;
    u16 Offset1;
}__attribute__((packed));

struct PeusdoDescriptor {
    u16 limit;
    u32 base;
}__attribute__((packed));

void kernel_initExceptions(void);

void kernel_loadGDT(const void*);
void kernel_loadLDT(const void*);
void kernel_loadIDT(const void*);

void __attribute__((cdecl)) kernel_storeGDT(const void*);

void kernel_setGDT(void*, u16, struct SegmentDescriptor*);
void kernel_setLDT(void*, u16, struct SegmentDescriptor*);
void kernel_setIDT(void*, u16, struct SegmentDescriptor*);


void kernel_setIDTEntry(struct InterruptDescriptor*, u32, u16, u8, u8);
void kernel_setGDTEntry(struct SegmentDescriptor*, u32, u32, u8, u8);





#endif