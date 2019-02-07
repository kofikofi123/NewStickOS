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

		temp++;
	}

	return NULL;
}

struct kernel_BMM* kernel_getEntry(u32 entry){
	if (entry >= _kernel_MemMapSize) return NULL;
	return (_kernel_MemMap + entry);
}

struct kernel_BMM* kernel_lookMemMapSB(u32 size, u32 after){
	struct kernel_BMM* temp = _kernel_MemMap;
	
	u32 end_u = 0;

	for (u32 i = 0; i < _kernel_MemMapSize; i++){
		end_u = (u32)temp->base + ((u32)temp->len - 1);

		if (after >= end_u && (end_u - after) >= size)
			return temp;
		temp++;
	}

	return NULL;
}

u32 kernel_getLargestAddr(){
	struct kernel_BMM* temp = _kernel_MemMap;

	u32 curAddr = 0;
	u32 otherAddr = 0;
	for (u32 i = 0; i < _kernel_MemMapSize; i++){
		otherAddr = (u32)temp->base + (u32)(temp->len - 1);

		if (otherAddr > curAddr)
			curAddr = otherAddr;
		temp++;
	}

	return curAddr;
}

inline u32 kernel_getEntryLength(){
	return _kernel_MemMapSize;
}

void kernel_debugMemMap(){
	struct kernel_BMM* temp = _kernel_MemMap;

	for (u32 i = 0; i < _kernel_MemMapSize; i++){
		kernel_printfBOCHS("{\n\tBase: %x\n\tSize: %x\n\tType: %d\n}\n", (u32)temp->base, (u32)temp->len, temp->type);
		temp++;
	}
}