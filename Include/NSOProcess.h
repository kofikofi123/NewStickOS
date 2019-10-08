#ifndef KERNEL_SCHED_H
#define KERNEL_SCHED_H

#include "NSOPaging.h"
#include "NSOThread.h"
#include "NSOMP.h"

struct kernel_ProcessInfo {
	const char* name;
	u32 pid;

	void* heap;
	u32* pageDirectory;

	u32 threadCount;
	struct kernel_ThreadInfo* threads;

	struct kernel_TSS* tss;

	struct kernel_ProcessInfo* next;
	struct kernel_ProccesInfo* prev;
};

struct kernel_ProcessInfo* kernel_createNewProcess();

#endif