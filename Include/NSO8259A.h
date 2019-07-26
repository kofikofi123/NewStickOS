#ifndef KERNEL_DEVICES_8259A_H
#define KERNEL_DEVICES_8259A_H

#include "DataType.h"

u8 kernel_init8259A();
void kernel_8259EOI(u8);
void kernel_mask8259INTS(u8, u8);

#endif