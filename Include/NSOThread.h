#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H


struct kernel_processInfo;

struct kernel_ThreadInfo {
	struct kernel_ProcessInfo* process;


	void* stack;
	void* kernelStack;

	struct {
		u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	}registers;


	struct kernel_ThreadInfo* next;
	struct kernel_ThreadInfo* prev;
};

struct kernel_ThreadInfo* kernel_createThread(struct kernel_ProcessInfo*);

#endif