#ifndef KERNEL_PIC_H
#define KERNEL_PIC_H

#include "DataType.h"

typedef void(*kernel_InterruptFunc)(struct kernel_IRegs); //prob not a good way]

void kernel_initIRQ();
void kernel_irqHandler(struct kernel_IRegs);
void kernel_requestIRQ(u8, kernel_InterruptFunc);
void kernel_freeIRQ(u8);

#endif