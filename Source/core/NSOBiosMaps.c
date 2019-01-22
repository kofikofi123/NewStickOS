#include "NSOBiosMaps.h"
#include "NSOBochs.h"
#include "DataType.h"

struct kernel_BMM* _kernel_MemMap;
u32 _kernel_MemMapSize;


/*
 * Function: kernel_initMemMapB
 * ----------------------------
 * This function setups the capability to lookup
 * BIOS memory maps.
 *
 *
 * TODO: Possibly re-order memory maps, since we are testing only on emulator prob is already order for us
 */
void kernel_initMemMapB(){
	_kernel_MemMapSize = *(u32*)(0x500);
	_kernel_MemMap = (struct kernel_BMM*)*(u32*)(0x518);
}


/*
 * Function: kernel_lookupMemMapB
 * ------------------------------
 * This function looks through the bios memory map 
 * and returns the map the address belongs to
 *
 * addr: The address to lookup
 *
 *
 * Return: Returns a pointer to the memory map
 */
struct kernel_BMM* kernel_lookupMemMapB(void* addr){
	struct kernel_BMM* temp = _kernel_MemMap;

	u64 addrVal = (u32)addr;
	u64 base = 0, end = 0;

	for (u32 i = 0; i < _kernel_MemMapSize; i++){
		base = temp->base;
		end = base + (temp->len - 1);

		if (base <= addrVal && end >= addrVal)
			return temp;
	}

	return NULL;
}