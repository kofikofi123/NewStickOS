#include "NSOIOAPIC.h"
#include "NSOAllocator.h"
#include "NSOPaging.h"


struct kernel_IOAPIC _kernel_baseIO;

static void _kernel_appendIOAPIC(struct kernel_IOAPIC*);


//static u32 _kernel_readIO(const struct kernel_IOAPIC*, u8);
//static void _kernel_writeIO(const struct kernel_IOAPIC*, u8, u32);

void kernel_newIOAPIC(u8 ioapicID, u32 ioapicAddr, u32 gsib){
	struct kernel_IOAPIC* io = kernel_malloc(sizeof(struct kernel_IOAPIC), 4);

	if (io == NULL) return;

	kernel_mapAddress(ioapicAddr, ioapicAddr, 0x02);

	u8* temp = (u8*)ioapicAddr;

	io->ioapicID = ioapicID;

	io->ioregsel = (u32*)temp;
	io->iowin = (u32*)(temp + 0x10);

	io->gsib = gsib;
	io->redirectionCount = (kernel_readIOAPIC(io, 0x01) >> 16) & 0xFF;
	io->version = kernel_readIOAPIC(io, 0x01) & 0xFF;

	io->next = NULL;
	

	_kernel_appendIOAPIC(io);

}

u32 kernel_readIOAPIC(const struct kernel_IOAPIC* io, u8 reg){
	*(io->ioregsel) = (u32)reg;
	return *(io->iowin);
}

void kernel_writeIOAPIC(const struct kernel_IOAPIC* io, u8 reg, u32 value){
	*(io->ioregsel) = (u32)reg;
	*(io->iowin) = value;
}

struct kernel_IOAPIC* kernel_getIOAPIC(u8 index){
	struct kernel_IOAPIC* last = _kernel_baseIO.next;

	while (index-- > 0)
		last = last->next;

	return last;
}

static void _kernel_appendIOAPIC(struct kernel_IOAPIC* device){
	struct kernel_IOAPIC* last = &_kernel_baseIO;

	while (last->next != NULL)
		last = last->next;

	last->next = device;
}

struct kernel_IOAPIC* kernel_getIOAPICByID(u8 ioI){
	struct kernel_IOAPIC* last = _kernel_baseIO.next;

	while (last != NULL){
		if (last->ioapicID == ioI)
			break;
		last = last->next;
	}

	return last;
}