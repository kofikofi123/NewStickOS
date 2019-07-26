#ifndef KERNEL_APIC_H
#define KERNEL_APIC_H

#include "NSOMP.h"

u8 kernel_hasAPIC();

void kernel_initAPIC();

u32 kernel_readAPICRegister(u16);
void kernel_writeAPICRegister(u16, u32);

void kernel_apicEOI();
#endif