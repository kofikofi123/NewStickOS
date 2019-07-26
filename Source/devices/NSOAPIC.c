#include "DataType.h"
#include "NSOAPIC.h"
#include "NSOPAllocator.h"
#include "NSOPaging.h"
#include "NSOCoreUtils.h"
#include "NSOBochs.h"

static u8* _kernel_apicPage = NULL;

//todo, make it friendly to MP config3`


void kernel_initAPIC(){
	void* new_page = kernel_allocatePage();

	if (new_page == NULL) kernel_panic("Unable to allocate page for APIC");

	u32 new_page32 = (u32)new_page;

	kernel_wrmsr(0x1B, new_page32 | (1 << 11));
	kernel_mapAddress(new_page32, new_page32, 0x02);

	_kernel_apicPage = new_page;
}

void kernel_initAPIC2(){
	u32 page32 = (u32)_kernel_apicPage;
	kernel_wrmsr(0x1B, page32 | (1 << 11));
	kernel_mapAddress(page32, page32, 0x02);
}

u8 kernel_hasAPIC(){
	u32 a = 1, b = 0, c = 0, d = 0;

	kernel_cpuid(&a, &b, &c, &d);
	return (d >> 9) & 0x01;
}

u32 kernel_readAPICRegister(u16 reg){
	return *(volatile u32*)(_kernel_apicPage + reg);
}

void kernel_writeAPICRegister(u16 reg, u32 value){
	*(volatile u32*)(_kernel_apicPage + reg) = value;
}

inline void kernel_apicEOI(){
	kernel_writeAPICRegister(0xB0, 0);
}