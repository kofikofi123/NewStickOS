#include "NSOSched.h"
#include "NSOThread.h"
#include "NSOBochs.h"

struct kernel_ProcessInfo _kernel_ReadyList;
struct kernel_ProcessInfo _kernel_WaitingList;
struct kernel_ProcessInfo _kernel_TerminateList; //tobo terminated list
struct kernel_ProcessInfo _kernel_RunningList;

static void _kernel_appendProcess(struct kernel_ProcessInfo*, struct kernel_ProcessInfo*);
static struct kernel_ProcessInfo* _kernel_getLastProcess(struct kernel_ProcessInfo*);
static u32 _kernel_getNumOfProcesses(struct kernel_ProcessInfo*);

void kernel_initScheduler(){

}

void kernel_scheduler(){}


inline void kernel_appendNewProcess(struct kernel_ProcessInfo* process){
	_kernel_appendProcess(&_kernel_ReadyList, process);
}


static u32 _kernel_getNumOfProcesses(struct kernel_ProcessInfo* list){
	u32 num = 0;
	while (list != NULL){
		list = list->next;
		num++;
	}
	return num;
}

static struct kernel_ProcessInfo* _kernel_getLastProcess(struct kernel_ProcessInfo* main){
	struct kernel_ProcessInfo* last = main;
	while (last->next != NULL)
		last = last->next;
	return last;
}

static void _kernel_appendProcess(struct kernel_ProcessInfo* main, struct kernel_ProcessInfo* process){
	struct kernel_ProcessInfo* last = _kernel_getLastProcess(main);

	last->next = process;
	process->prev = last;
}

