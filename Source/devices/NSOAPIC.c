#include "DataType.h"
#include "NSOAPIC.h"
#include "NSOPAllocator.h"
#include "NSOPaging.h"
#include "NSOCoreUtils.h"
#include "NSOBochs.h"

//static void* _kernel_apicPage = NULL;

//todo, make it friendly to MP config3`


void kernel_initAPIC(){
	void* new_page = kernel_allocatePage();

	if (new_page == NULL) kernel_panic("Unable to allocate page for APIC");

	u32 new_page32 = (u32)new_page;

	kernel_wrmsr(0x1B, new_page32 | (1 << 11));
	kernel_mapAddress(new_page32, new_page32, 0x02);

	kernel_printfBOCHS("%x\n", *(u32*)(new_page32 + 0x20));
}

u8 kernel_hasAPIC(){
	u32 a = 1, b = 0, c = 0, d = 0;

	kernel_cpuid(&a, &b, &c, &d);
	return (d >> 9) & 0x01;
}
