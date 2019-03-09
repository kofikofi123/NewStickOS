#include "NSOCacheControl.h"
#include "NSOCoreUtils.h"

u8 kernel_isPATSupported(){
	u32 a = 1, b = 0, c = 0, d = 0;
	kernel_cpuid(&a, &b, &c, &d);
	return (d >> 16) & 0x01;
}


void kernel_initCacheControl(){
	
}