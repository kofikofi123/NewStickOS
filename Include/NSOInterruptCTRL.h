#ifndef KERNEL_INTERRUPT_CONTROL
#define KERNEL_INTERRUPT_CONTROL

#include "DataType.h"
#include "NSOInterrupts.h"


void kernel_initateInterruptController();
void kernel_isrHandler(struct kernel_IRegs);
void* kernel_requestIRQ(u32, char*, kernel_interruptHandle, u16);

void kernel_overrideIsaIRQ(u8, u32, u16);

#endif