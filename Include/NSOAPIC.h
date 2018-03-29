#ifndef _NSOAPIC_H
#define _NSOAPIC_H

#include "DataType.h"

u8 kernel_checkAPIC(void);
void kernel_enableAPIC(void);
void kernel_disableAPIC(void);
void* kernel_getAPICAddress(void);

#endif 