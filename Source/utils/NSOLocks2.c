#include "NSOLocks.h"
#include "NSOAllocator.h"

inline kernel_omniLock kernel_createLock(){
	return kernel_malloc(sizeof(u32), 4);
}