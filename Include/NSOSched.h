#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include "NSOProcess.h"

void kernel_initScheduler();
void kernel_appendNewProcess(struct kernel_ProcessInfo*);

#endif