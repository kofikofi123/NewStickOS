#include "NSOBochs.h"
#include "NSOAllocator.h"
#include "NSOBiosMaps.h"
#include "NSOPAllocator.h"

void __attribute__((section("._main"))) kernel_main() {
	kernel_initMemMapB();
	kernel_initPageAllocator();

	
	while (1){}	
}