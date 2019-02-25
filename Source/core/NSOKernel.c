#include "NSOBochs.h"
#include "NSOAllocator.h"
#include "NSOBiosMaps.h"
#include "NSOPAllocator.h"
#include "NSOInterrupts.h"
#include "NSOCoreUtils.h"
#include "NSOPCI.h"
#include "NSOPaging.h"

extern u32 kernel_end;
void __attribute__((section("._main"))) kernel_main() {
	kernel_initMemMapB();
	kernel_initPageAllocator();
	kernel_initInterrupts();
	kernel_getMaxCpuid();
	kernel_initPaging();
	kernel_mapIdentity(0, ((u32)(&kernel_end) & ~(0x3FF))/0x1000, 0x02);
	kernel_pciDebug(0, 1, 1);
	kernel_updatePaging();
	kernel_enablePaging();


	while (1){}	
}