#include "NSOLocks.h"
#include "NSOAllocator.h"

inline kernel_omniLock kernel_createLock(){
	return kernel_calloc(sizeof(u32), 4);
}