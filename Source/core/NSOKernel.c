#include "NSOBochs.h"
#include "NSOAllocator.h"
#include "NSOBiosMaps.h"
#include "NSOPAllocator.h"
#include "NSOInterrupts.h"
#include "NSOCoreUtils.h"
#include "NSOPCI.h"
#include "NSOPaging.h"
#include "NSOCacheControl.h"

extern u32 kernel_end;
void __attribute__((section("._main"))) kernel_main() {
	u32 _kernel_end = (u32)&kernel_end;

	
	kernel_initMemMapB();

	kernel_initPageAllocator();

	kernel_initInterrupts();

	//get max cpuid's
	kernel_getMaxCpuid();

	//init paging
	kernel_initPaging();
	kernel_mapIdentity(0, _kernel_end, 0x02);
	kernel_updatePaging();
	kernel_enablePaging();
	//

	{
		u32 maxAddr = kernel_getLargestAddr() & ~(0xFFF);
		kernel_mapAddress(maxAddr, maxAddr, 0x02);

	}
	
	while (1){}	
}