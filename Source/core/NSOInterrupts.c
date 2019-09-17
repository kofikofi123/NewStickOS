#include "NSOInterrupts.h"
#include "NSOBochs.h"


struct kernel_IDT __attribute__((aligned(8))) _kernel_local_IDT[256] = {0};
struct kernel_DescriptorPointers _kernel_local_pointer;

static void _kernel_encodeIDT(struct kernel_IDT*, u32, u16, u8, u16);
//static void _kernel_int_stub();
//static void _kernel_int();

extern void kernel_isr0();
//extern void kernel_isr1();
//extern void kernel_isr3();
extern void kernel_isr8();
extern void kernel_isr13();
extern void kernel_isr14();




void kernel_initInterrupts(){
	kernel_setupTrapGate(kernel_isr0, 0x08, 0, 0, 1, 1);
	kernel_setupTrapGate(kernel_isr8, 0x08, 8, 0, 1, 1);

	kernel_setupTrapGate(kernel_isr13, 0x08, 13, 0, 1, 1);
	kernel_setupTrapGate(kernel_isr14, 0x08, 14, 0, 1, 1);

	extern u32 kernel_isrstub_start;
	extern u32 kernel_isrstub_end;

	u8 *stubs_start = (u8*)&kernel_isrstub_start, *stubs_end = (u8*)&kernel_isrstub_end;
	u8 temp_vector = 0x20;
	
	while (stubs_start < stubs_end){
		kernel_setupTrapGate(stubs_start, 0x08, temp_vector, 0, 1, 1);
		stubs_start += 0x0C;
		temp_vector += 1;
	}

	
	_kernel_local_pointer.size = sizeof(_kernel_local_IDT) - 1;
	_kernel_local_pointer.location = (u32)_kernel_local_IDT;
	kernel_flushIDT(&_kernel_local_pointer);
}

void kernel_setupInterruptGate(void* address, u16 segment, u8 entry, u8 dpl, u8 d, u8 p){
	_kernel_encodeIDT(&_kernel_local_IDT[entry], (u32)address, segment, KERNEL_ENCODE_IDT_FLAGS(p, dpl, d), KERNEL_IDT_INT);
}

void kernel_setupTrapGate(void* address, u16 segment, u8 entry, u8 dpl, u8 d, u8 p){
	_kernel_encodeIDT(&_kernel_local_IDT[entry], (u32)address, segment, KERNEL_ENCODE_IDT_FLAGS(p, dpl, d), KERNEL_IDT_TRP);
}


static void _kernel_encodeIDT(struct kernel_IDT* entry, u32 addr, u16 selector, u8 flag, u16 type){
	entry->offset0 = (addr & 0xFFFF);
	entry->offset1 = ((addr >> 16) & 0xFFFF);

	entry->segment = selector;
	entry->type = type | (flag << 8);
}

void kernel_flushIDT(const struct kernel_DescriptorPointers* ptr){
	__asm__ volatile ("push eax\n\tmov eax, %0\n\tlidt [eax]\n\tpop eax"
					  :
					  : "r" (ptr)
					  : "memory", "eax");
}
/*
static void _kernel_int_stub(){
	kernel_panic("Stub");
}

static void _kernel_int(){
	kernel_printfBOCHS("Interrupt occured\n");
}*/
