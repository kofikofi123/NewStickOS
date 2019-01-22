#include "NSOBochs.h"
#include "NSOAllocator.h"
#include "NSOBiosMaps.h"

void __attribute__((section("._main"))) kernel_main() {
	kernel_initMemMapB();
	kernel_init_allocation();
	
	kernel_alloc_debug();
	void* temp = kernel_malloc(5);
	kernel_alloc_debug();
	void* temp2 = kernel_malloc(5);
	kernel_alloc_debug();

	kernel_free(temp);
	kernel_alloc_debug();
	kernel_free(temp2);
	kernel_alloc_debug();
	while (1){}	
}