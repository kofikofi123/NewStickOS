#ifndef KERNEL_PCI_H
#define KERNEL_PCI_H

#include "DataType.h"

u8 kernel_pciReadConfig8(u8, u8, u8, u8, u8);
u16 kernel_pciReadConfig16(u8, u8, u8, u8, u8);
u32 kernel_pciReadConfig32(u8, u8, u8, u8);

void kernel_pciDebug(u8, u8, u8);
void kernel_enumeratePCI();
struct kernel_pciDevice* kernel_pciSearchDevice(u16, u16);

struct kernel_pciBus {
	u16 busNumber;

	struct kernel_pciBus* next;
	struct kernel_pciDevice* devices;
};


struct kernel_pciDevice {
	u16 vendorID;
	u16 deviceID;
	u8 busNumber;
	u8 deviceNumber;
	u8 functionNumber;
	u8 flags;

	struct kernel_pciDevice* next;
};

#endif
