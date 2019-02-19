#include "NSOBochs.h"
#include "NSOAllocator.h"
#include "NSOBiosMaps.h"
#include "NSOPAllocator.h"
#include "NSOInterrupts.h"
#include "NSOCoreUtils.h"
#include "NSOPCI.h"
#include "NSOPaging.h"

void __attribute__((section("._main"))) kernel_main() {
	kernel_initMemMapB();
	kernel_initPageAllocator();
	kernel_initInterrupts();
	kernel_getMaxCpuid();
	kernel_initPaging();

	while (1){}	
}