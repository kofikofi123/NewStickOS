#include "NSOMP.h"
#include "NSOACPI.h"
#include "NSOAPIC.h"
#include "NSOCoreUtils.h"
#include "NSOAllocator.h"

static struct kernel_ProcessorInfo _kernel_bprocessors;
static u8 _kernel_numProcessors = 0;

static struct kernel_ProcessorInfo* _kernel_createProcessor(u8, u8, u8);
static void _kernel_appendProcessor(struct kernel_ProcessorInfo*);

void kernel_newProcessor(u8 acpiID, u8 apicID, u8 usable){
	struct kernel_ProcessorInfo* cpu = _kernel_createProcessor(acpiID, apicID, usable);

	_kernel_appendProcessor(cpu);
}

struct kernel_ProcessorInfo* kernel_getProc(u8 index){
	if (index >= _kernel_numProcessors) return NULL;

	struct kernel_ProcessorInfo* temp = _kernel_bprocessors.next;

	while ((index-- > 0)){
		temp = temp->next;
	}

	return temp;
}

struct kernel_ProcessorInfo* kernel_currentCPU(){
	return kernel_getProcByAPIC(kernel_readAPICRegister(0x20) >> 24);
}

struct kernel_ProcessorInfo* kernel_getProcByAPIC(u8 apicID){

	struct kernel_ProcessorInfo* temp = _kernel_bprocessors.next;

	while (temp != NULL){
		if (temp->apicID == apicID)
			return temp;
		temp++;
	}

	return temp;
}

void kernel_setupTSS(struct kernel_ProcessorInfo* cpu){
	kernel_free(cpu->processorTSS);

	struct kernel_TSS* tss = kernel_malloc(sizeof(struct kernel_TSS), 4);

	if (tss != NULL)
		kernel_memset(tss, 0, sizeof(struct kernel_TSS));

	cpu->processorTSS = tss;
}

inline u8 kernel_getNumOfProc(){
	return _kernel_numProcessors;
}


static void _kernel_appendProcessor(struct kernel_ProcessorInfo* cpu){
	struct kernel_ProcessorInfo* last = &_kernel_bprocessors;

	while (last->next != NULL)
		last = last->next;

	last->next = cpu;
}

static struct kernel_ProcessorInfo* _kernel_createProcessor(u8 acpiID, u8 apicID, u8 usable){
	struct kernel_ProcessorInfo* processor = kernel_malloc(sizeof(struct kernel_ProcessorInfo), 1);

	if (processor != NULL){
		kernel_memset(processor, 0, sizeof(struct kernel_ProcessorInfo));
		processor->apicID = apicID;
		processor->acpiID = acpiID;
		processor->isUsable = usable;
		processor->next = NULL;
	}

	return processor;
}