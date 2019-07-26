#ifndef KERNEL_IO_APIC_H
#define KERNEL_IO_APIC_H

#include "DataType.h"

struct kernel_IOAPIC {
	u32 ioapicID;

	u32 volatile * ioregsel;
	u32 volatile * iowin;

	u16 gsib;
	u8 redirectionCount;
	u8 version;

	struct kernel_IOAPIC* next;
};

struct kernel_RedirectionIRQ {
	
};

void kernel_newIOAPIC(u8, u32, u32);

struct kernel_IOAPIC* kernel_getIOAPIC(u8);
struct kernel_IOAPIC* kernel_getIOAPICByID(u8);

u32 kernel_readIOAPIC(const struct kernel_IOAPIC*, u8);
void kernel_writeIOAPIC(const struct kernel_IOAPIC*, u8, u32);


#endif