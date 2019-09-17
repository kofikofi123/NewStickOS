#ifndef KERNEL_IO_APIC_H
#define KERNEL_IO_APIC_H

#define KERNEL_IOAPIC_FIXED 0x00
#define KERNEL_IOAPIC_LOWPRIO 0x01
#define KERNEL_IOAPIC_SMI 0x02
#define KERNEL_IOAPIC_NMI 0x04
#define KERNEL_IOAPIC_INIT 0x05
#define KERNEL_IOAPIC_EXTINT 0x07

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
	u8 interruptVector;
	u8 deliveryMode : 3;
	u8 destinationMode : 1;
	u8 deliveryStatus : 1;
	u8 intpol : 1;
	u8 remoteIRR : 1;
	u8 triggerMode : 1;
	u8 interruptMask : 1;
	u64 reserved : 39;
	u8 destinationField;
};

void kernel_newIOAPIC(u8, u32, u32);

struct kernel_IOAPIC* kernel_getIOAPIC(u8);
struct kernel_IOAPIC* kernel_getIOAPICByID(u8);

u32 kernel_readIOAPIC(const struct kernel_IOAPIC*, u8);
void kernel_writeIOAPIC(const struct kernel_IOAPIC*, u8, u32);

void kernel_readRedirectionTable(const struct kernel_IOAPIC*, struct kernel_RedirectionIRQ*, u8);
void kernel_writeRedirectionTable(const struct kernel_IOAPIC*, struct kernel_RedirectionIRQ*, u8);

#endif