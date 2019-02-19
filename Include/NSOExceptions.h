#ifndef KERNEL_EXCEPIONS_H
#define KERNEL_EXCEPIONS_H

#include "DataType.h"

void kernel_divEX();
void kernel_breakpointEx();
void kernel_doubleFaultEX(struct kernel_IRegs);
void kernel_generalProtectionEX(struct kernel_IRegs);
void kernel_pagingEX();

#endif