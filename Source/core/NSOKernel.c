#include "NSOBochs.h"
#include "NSOAllocator.h"
#include "NSOBiosMaps.h"
#include "NSOPAllocator.h"
#include "NSOInterrupts.h"
#include "NSOCoreUtils.h"
#include "NSOPCI.h"
#include "NSOPaging.h"
#include "NSOACPI.h"
#include "NSOCacheControl.h"

extern u32 kernel_end;
void __attribute__((section("._main"))) kernel_main() {
	u32 _kernel_end = (u32)&kernel_end;

	//grab memory pges
	kernel_initMemMapB();
	//init page allocator
	kernel_initPageAllocator();
	//init base interrupts
	kernel_initInterrupts();

	//get max cpuid's
	kernel_getMaxCpuid();

	//init paging
	kernel_initPaging();
	kernel_mapIdentity(0, _kernel_end, 0x02);
	kernel_updatePaging();
	kernel_enablePaging();
	
	//init kernel heaap
	kernel_initAllocation();
	
	kernel_initACPI();
	{
		void* fadt = kernel_findACPITable("FACP");
	
		if (fadt == NULL)
			kernel_panic("Unable to find FACP");

		u32 dsdtAddr = (u32)(*(u64*)(fadt + 140));

		if (dsdtAddr == 0)
			dsdtAddr = *(u32*)(fadt + 40);

		extern struct kernel_ACPIScope kernel_rootNamespace;
		kernel_rootNamespace.SDT = (struct kernel_ACPIHeader*)dsdtAddr;
		kernel_loadAML(&kernel_rootNamespace, (u32*)dsdtAddr);
		kernel_debugACPITree(&kernel_rootNamespace);
	}

	while (1){}	
}