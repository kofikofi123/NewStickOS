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
	u32 _kernel_end = (u32)&kernel_end;
	kernel_initMemMapB();
	kernel_initPageAllocator();
	kernel_initInterrupts();
	kernel_getMaxCpuid();
	kernel_initPaging();
	kernel_mapIdentity(0, _kernel_end, 0x02);
	kernel_updatePaging();
	kernel_enablePaging();

	{
		void* temp = kernel_vAllocatePage(0x50000, 5, 0x02);
		kernel_memset(temp, 0xFF, 5 * 0x1000);
		__asm__("xchg bx, bx");
		kernel_vFreePage(0x50000, 5);

	}
	
	
	while (1){}	
}