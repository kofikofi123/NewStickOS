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
#include "NSOLocks.h"

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
	kernel_printfBOCHS("Is it hanging!\n");
	/*
	kernel_omniLock lock = kernel_createLock();

	kernel_printfBOCHS("Test: %b\n", kernel_testSpinlock(lock));
	kernel_acquireSpinlock(lock);
	kernel_printfBOCHS("Test: %b\n", kernel_testSpinlock(lock));
	kernel_releaseLock(lock);
	kernel_printfBOCHS("Test: %b\n", kernel_testSpinlock(lock));*/
	
	while (1){}	
}