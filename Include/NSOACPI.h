#ifndef KERNEL_ACPI_H
#define KERNEL_ACPI_H

#include "DataType.h"

#ifndef _NOS
#define _NOS
#endif

#include "acpica/acpi.h"

struct kernel_AcpiGAS {
	u8 AddressSpaceID;
	u8 RegisterBitWidth;
	u8 RegisterBitOffset;
	u8 AccessSize;
	u64 Address;
};



#endif
