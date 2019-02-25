#ifndef KERNEL_PCI_H
#define KERNEL_PCI_H

#include "DataType.h"

void kernel_pciFindDevice(u32, u32, u8*, u8*);

u8 kernel_pciReadConfig8(u8, u8, u8, u8, u8);
u16 kernel_pciReadConfig16(u8, u8, u8, u8, u8);
u32 kernel_pciReadConfig32(u8, u8, u8, u8);

void kernel_pciDebug(u8, u8, u8);

#endif
