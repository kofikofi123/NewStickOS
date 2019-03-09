#ifndef KERNEL_INTERRUPT_H
#define KERNEL_INTERRUPT_H

#include "DataType.h"

#define KERNEL_DECLAR_INTERRUPT(NAME) \
	void __attribute__((cdecl)) kernel_ ## NAME (struct kernel_IRegs)
#define KERNEL_DEFINE_INTERRUPT(NAME) \
	void __attribute__((cdecl)) kernel_ ## NAME (struct kernel_IRegs registers)
#define KERNEL_ENCODE_IDT_FLAGS(p, dpl, d) \
	0 | (p << 7) | (dpl << 6) | (d << 3)

#define KERNEL_IDT_INT (u16)0x0600
#define KERNEL_IDT_TRP (u16)0x0700

struct kernel_IDT {
	u16 offset0;
	u16 segment;
	u16 type;
	u16 offset1;
};


void kernel_initInterrupts();

void kernel_setupInterruptGate(void*, u16, u8, u8, u8, u8);
void kernel_setupTrapGate(void*, u16, u8, u8, u8, u8);
void kernel_flushIDT(const struct kernel_DescriptorPointers*);


#endif