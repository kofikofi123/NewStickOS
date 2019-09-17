#ifndef KERNEL_LOCKS_H
#define KERNEL_LOCKS_H


#include "DataType.h"

typedef u32* kernel_omniLock;

kernel_omniLock kernel_createLock();

void kernel_acquireSpinlock(kernel_omniLock);
u8 kernel_testSpinlock(kernel_omniLock);

u8 kernel_acquireMutex(kernel_omniLock);
u8 kernel_tryMutex(kernel_omniLock);
void kernel_releaseLock(kernel_omniLock);



#endif