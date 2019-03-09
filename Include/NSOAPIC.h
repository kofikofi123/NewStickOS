#ifndef KERNEL_APIC_H
#define KERNEL_APIC_H

u8 kernel_hasAPIC();
void kernel_initAPIC();

u32 kernel_readRegisters(u16);
void kernel_writeRegisters(u16, u32);
#endif