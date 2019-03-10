#include "NSOACPI.h"
#include "NSOPaging.h"
#include "NSOBochs.h"
#include "NSOCoreUtils.h"



void kernel_initACPI(){
	//search EBDA
	u32 ebdaBase = (u32)(*(u16*)(0x40E)) << 4;
	
	if (!kernel_mapAddress(ebdaBase, ebdaBase, 0x02))
		kernel_panic("Unable to map ACPI");

	const char* rsdsig = "RSD PTR ";

	void* start = (void*)ebdaBase;
}