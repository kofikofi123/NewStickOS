#ifndef _NSOMSR_H
#define _NSOMSR_H

#include "DataType.h"


u64 kernel_rdmsr(u32);
void kernel_wrmsr(u32, u64);


#endif