#ifndef KERNEL_ATA_H
#define KERNEL_ATA_H

#include "DataType.h"
#include "NSOPCI.h"

#define KERNEL_IDECHANNEL_PRIMARY_COMMAND 0x00
#define KERNEL_IDECHANNEL_PRIMARY_CONTROL 0x01
#define KERNEL_IDECHANNEL_SECONDARY_COMMAND 0x02
#define KERNEL_IDECHANNEL_SECONDARY_CONTROL 0x03



struct kernel_ideChannel {
	struct kernel_pciDevice* pciDevice;
	u32 ioBases[4];
	u32 progIF;
};

struct kernel_ataDevice {
	u8 type;

	struct kernel_ideChannel* channel;
	struct kernel_ataDevice* next;
};

void kernel_detectATADevices();

#endif