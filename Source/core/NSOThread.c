#include "NSOProcess.h"
#include "NSOAllocator.h"
#include "NSOPAllocator.h"
#include "NSOThread.h"


struct kernel_ThreadInfo* kernel_createThread(struct kernel_ProcessInfo* process){
	struct kernel_ThreadInfo* thread = kernel_malloc(sizeof(struct kernel_ThreadInfo), 4);
	void* stack = kernel_allocatePage();

	if (!thread || !stack){
		kernel_free(thread);
		kernel_free(stack);
		return NULL;
	}

	thread->stack = stack;
	return thread;
}