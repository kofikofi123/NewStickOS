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
#include "NSOACPI.h"

extern u32 kernel_end;
void __attribute__((section("._main"))) kernel_main() {
	u32 _kernel_end = (u32)&kernel_end;

	kernel_printfBOCHS("Kernel end: %x\n", _kernel_end);
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
	kernel_initPageAllocator2();
	
	kernel_updatePaging();
	kernel_enablePaging();
	
	//init kernel heaap
	kernel_initAllocation();
	
	
	//acpica 
	if (ACPI_FAILURE(AcpiInitializeSubsystem()))
		kernel_panic("Unable to init acpica subsystems");
	
	if (ACPI_FAILURE(AcpiInitializeTables(NULL, 16, FALSE)))
		kernel_panic("Unable to init acpica tables");
	
	ACPI_TABLE_HEADER* tbl = NULL;

	AcpiGetTable("FACP", 1, &tbl);

	kernel_printfBOCHS("test: %x\n", (u32)tbl);

	while (1){}	
}