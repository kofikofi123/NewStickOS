#include "NSOProcess.h"
#include "NSOAllocator.h"

struct kernel_ProcessInfo* kernel_createNewProcess(u32* oPD){
	struct kernel_ProcessInfo* process = kernel_malloc(sizeof(struct kernel_ProcessInfo), 4);
	u32* pd = !oPD ? oPD : kernel_createVirtualAddressSpace();

	if (!process || !pd){
		kernel_free(process);
		kernel_free(pd);
		return NULL;
	}


	process->name = NULL;
	process->pid = 0;
	process->pageDirectory = pd;
	process->threads = NULL;

	return process;
}