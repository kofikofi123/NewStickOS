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
#include "NSOMP.h"
#include "NSOHPET.h"
#include "NSOIOAPIC.h"
#include "NSOInterruptCTRL.h"
#include "NSOPIT.h"
#include "NSOProcess.h"
#include "NSOThread.h"
#include "NSOSched.h"

extern u32 kernel_end;

//static void _kernel_apicTEST();

void __attribute__((section("._main"))) kernel_main() {
	u32 _kernel_end = (u32)&kernel_end;
	kernel_cli();
	//kernel_printfBOCHS("Kernel end: %x\n", _kernel_end);
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
	
	{
		/*char test[4] = {'k', 'o', 'f', 'i'};
		kernel_printfBOCHS("Combo tester: %#x | v%.2d | %s | %b\n", 0xDEADBEEF, 904, "kofi", 1);
		kernel_printfBOCHS("Combo tester2: %#8.8x\n", 0xFF);
		kernel_printfBOCHS("Combo tester3: %.4s\n", test);
		kernel_printfBOCHS("Combo tester4: %#8.8x%#x (v%.2d %.4s)", 0xDEAFBEAD, 0x20, 32, test);*/
	}
	
	//acpica 
	if (ACPI_FAILURE(AcpiInitializeSubsystem()))
		kernel_panic("Unable to init acpica subsystems");
	
	if (ACPI_FAILURE(AcpiInitializeTables(NULL, 16, TRUE)))
		kernel_panic("Unable to init acpica tables");

	{
		ACPI_TABLE_HEADER* madt = NULL;

		if (ACPI_FAILURE(AcpiGetTable("APIC", 1, &madt))){
			kernel_printfBOCHS("%x\n", madt);
			kernel_panic("Can't find MADT table");
		}

		///////////////////////////////////_|_//////////////////////////////////
		///////////////////////////////////_|_/////////////////////////////////

		u8* madt_base8 = (u8*)madt;

		u8* madt_lead = madt_base8 + sizeof(ACPI_TABLE_HEADER) + 8;
		u8* madt_end = madt_base8 + madt->Length;

		//struct kernel_IOAPIC* ioapic = NULL;
		//struct kernel_RedirectionIRQ* redirection = kernel_malloc(sizeof(struct kernel_RedirectionIRQ), 4);

		while (madt_lead < madt_end){
			u8 type = *(madt_lead);
			u8 length = *(madt_lead + 1);

			if (type == 0){
				kernel_newProcessor(*(madt_lead + 2), *(madt_lead + 3), *(u32*)(madt_lead + 4) & 0x01);
			}else if (type == 1){
				kernel_newIOAPIC(*(madt_lead + 2), *(u32*)(madt_lead + 4), *(u32*)(madt_lead + 8));
			}else if (type == 2){
				kernel_overrideIsaIRQ(*(madt_lead + 3), *(u32*)(madt_lead + 4), *(u16*)(madt_lead + 8));
			}

			madt_lead = madt_lead + length;

			AcpiPutTable(madt);
		}

		//kernel_free(redirection);
	}


	kernel_initateInterruptController();

	//loops through the pci buses and generates nodes for each device present
	kernel_enumeratePCI();
	kernel_initPIT();
	//kernel_initHPET();

	kernel_sti();

	kernel_detectATADevices();
	//kernel_initScheduler();

	while (1){}
}

/*static void _kernel_apicTEST(struct kernel_IRegs* regs){
	kernel_printfBOCHS("Testing\n");
}*/