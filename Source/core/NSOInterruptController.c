#include "NSOInterruptCTRL.h"
#include "NSOACPI.h"
#include "NSOAPIC.h"
#include "NSO8259A.h"
#include "NSOCoreUtils.h"
#include "NSOInterrupts.h"
#include "NSOBochs.h"
#include "NSOAllocator.h"

static u8 _kernel_using_APIC = 0;

struct _kernel_irqNode {
	char* deviceName;
	kernel_interruptHandle handler;
	u16 vector;
	u16 flags;
	struct _kernel_irqNode* next;
};


struct _kernel_irqNode _kernel_irqBaseNode = {
	.handler = NULL,
	.vector = 0,
	.next = NULL
};

//saw this on linux sources, looked nice
static u32 _kernel_isaIrqMappings[16] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static u16 _kernel_isaMPS[16] = {0};

static void _kernel_setupAPIC();
static void _kernel_setup8259A();
static struct _kernel_irqNode* _kernel_allocateIRQ(u32, char*, kernel_interruptHandle, u16);
static void _kernel_spurINT(struct kernel_IRegs);
static void _kernel_emitEOI(u32);

void kernel_initateInterruptController(){
	u8 HAS_APIC = kernel_hasAPIC();
	u8 HAS_8259 = 0;

	ACPI_TABLE_HEADER* header = NULL;
	ACPI_STATUS temp;
	if ((temp = ACPI_FAILURE(AcpiGetTable("APIC", 1, &header)))){
		kernel_printfBOCHS("%s\n", AcpiFormatException(temp));
		kernel_panic("Couldn't find MADT table");
	}

	u32* MADT = ((void*)header + sizeof(ACPI_TABLE_HEADER));

	//u32 local_APIC = *(MADT);
	u32 flags = *(MADT + 1);

	HAS_8259 = (flags & 0x01);


	if (HAS_APIC){
		if (HAS_8259)
			_kernel_setupAPIC();
		kernel_printfBOCHS("Using APIC for interrupt controller\n");
		_kernel_using_APIC = 1;
	}else{
		_kernel_setup8259A();
		kernel_printfBOCHS("Using 8259A for interrupt controller\n");
	}
}

inline void kernel_overrideIsaIRQ(u8 isaIRQ, u32 gsibIRQ, u16 mpsFlags){
	_kernel_isaIrqMappings[isaIRQ] = gsibIRQ;
	_kernel_isaMPS[isaIRQ] = mpsFlags;
}

static void _kernel_setup8259A(){
	kernel_init8259A();
}

static void _kernel_setupAPIC(){
	kernel_mask8259INTS(0xFF, 0xFF);
	kernel_init8259A();
	kernel_initAPIC();

	//u32 acpi_version_reg = kernel_readAPICRegister(0x30);

	kernel_setupTrapGate(_kernel_spurINT, 0x08, 0xFF, 0, 1, 1);
	
	kernel_writeAPICRegister(0xF0, 0xFF | (1 << 8));
}

static struct _kernel_irqNode* _kernel_allocateIRQ(u32 vector, char* deviceName, kernel_interruptHandle handler, u16 flag){
	struct _kernel_irqNode* node = kernel_malloc(sizeof(struct _kernel_irqNode), 4);

	if (node != NULL){
		node->vector = vector;
		node->flags = flag;
		node->handler = handler;
		node->deviceName = deviceName;
		node->next = NULL;
	}

	return node;
}

static void _kernel_appendIRQ(struct _kernel_irqNode* irq){
	struct _kernel_irqNode* node = &_kernel_irqBaseNode;
	//struct _kernel_irqNode* temp = NULL;

	while (node->next != NULL){
		node = node->next;
	}

	node->next = irq;
}


static void _kernel_spurINT(struct kernel_IRegs iregs){
	kernel_printfBOCHS("Spurious interrupt detected\n");
}

static struct _kernel_irqNode* _kernel_searchIRQ(u32 vector){
	struct _kernel_irqNode* node = _kernel_irqBaseNode.next;


	while (node != NULL){
		if (node->vector == vector)
			break;
		node = node->next;
	}

	return node;
}

static void _kernel_emitEOI(u32 vector){
	if (_kernel_using_APIC)
		kernel_apicEOI();
	else
		kernel_8259EOI(vector);
}

void* kernel_requestIRQ(u32 vector, char* deviceName, kernel_interruptHandle handler, u16 flag){
	struct _kernel_irqNode* node = _kernel_allocateIRQ(vector, deviceName, handler, flag);

	if (node == NULL)
		return NULL;

	_kernel_appendIRQ(node);

	return (void*)1; //fornow
}

void kernel_isrHandler(struct kernel_IRegs regs){//todo: check for spurious interrupts
	u32 vector = regs.error_code;

	struct _kernel_irqNode* node = _kernel_searchIRQ(vector);

	if (node != NULL)
		node->handler(&regs);

	_kernel_emitEOI(vector);
	
}