#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include "DataType.h"

#define KERNEL_OSNAME "NewStickOS 3.0"

//Output functions
void kernel_out8(u16, u8);
void kernel_out16(u16, u16);
void kernel_out32(u16, u32);

//Input functions
u8 kernel_in8(u16);
u16 kernel_in16(u16);
u32 kernel_in32(u16);

//Flag manip functions
void kernel_sti();
void kernel_cli();
u32 kernel_getEFLAGS();
void kernel_setEFLAGS(u32);

//others
void kernel_panic(const char*) __attribute__((noreturn));
void kernel_hlt();
void kernel_cpuid(u32*, u32*, u32*, u32*);
void kernel_getMaxCpuid();
void kernel_memset(void*, u8, u32);
void kernel_memcpy(void*, const void*, u32);
s8 kernel_memcmp(const void*, const void*, u32);
u8 kernel_checkStandardCpuid(u32);
u8 kernel_checkExtendedCpuid(u32);
u8 kernel_isCpuidSupported();
void kernel_wrmsr(u32, u64);
u64 kernel_rdmsr(u32);
u8 kernel_calculateChecksum(void*, u32);
//unsafe Basis basic varadic impl


#define kernel_va_start(va) \
	__asm__ volatile("lea eax, [ebp + 12]\n\tmov %0, eax" \
			: "=r" (va) \
			: \
			: "eax");
#endif

#define kernel_get_next_address(v) \
	__asm__("mov eax, [ebp + 4]\n\tmov %0, eax\n" \
				: "=r" (v));

