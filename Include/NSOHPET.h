#ifndef KERNEL_HPET_H
#define KERNEL_HPET_H

#include "DataType.h"

#define KERNEL_HPET_TIMER0 0x100
#define KERNEL_HPET_TIMER1 0x120
#define KERNEL_HPET_TIMER2 0x140

void kernel_initHPET();
u8 kernel_getNumOfTimers();
u32 kernel_getTimerFreq(u8);
u8 kernel_setupTimer(u32);

#endif