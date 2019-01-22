#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include "DataType.h"

//Output functions
void kernel_outb(u16, u8);
void kernel_outw(u16, u16);
void kernel_outdw(u16, u32);

//Input functions
u8 kernel_inb(u16);
u16 kernel_inw(u16);
u32 kernel_indw(u16);


//unsafe Basis basic varadic impl

#define kernel_va_start(va) \
	__asm volatile("lea eax, [ebp + 12]\n\tmov %0, eax" \
			: "=r" (va) \
			: \
			: "eax");
#endif

//EOF kernel data variable
