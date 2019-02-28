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
	//kernel_pciDebug(0, 1, 1);
	__asm__("xchg bx, bx");
	kernel_updatePaging();
	kernel_enablePaging();

	{
		void* test = kernel_allocatePage();
		//kernel_printfBOCHS("OKR\n");
		if (test == NULL) kernel_panic(" ");
		kernel_printfBOCHS("Addr: %x\n", (u32)test);
		kernel_mapAddress((u32)test, (u32)test, 0x02);
		//kernel_memset(test, 0xFF, 4096);
	}
	int a = 32;

	while (1){}	
}