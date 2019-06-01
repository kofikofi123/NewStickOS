#include "NSOExceptions.h"
#include "NSOCoreUtils.h"
#include "NSOBochs.h"

void kernel_divEX(){
	kernel_panic("Divide by 0 fault");
}


void kernel_generalProtectionEX(struct kernel_IRegs iregs){
	u32 error_code = iregs.error_code;
	u8 ext = error_code & 0x01;
	u8 idt = (error_code >> 1) & 0x01;
	u8 ti = (error_code >> 2) & 0x01;
	u16 segment = (error_code >> 3) & 0x1FFF;

	kernel_printfBOCHS("Ext: %x\nIdt: %x\nti: %x\nsegment: %x", ext, idt, ti, segment);
	kernel_panic("General protection exception");
}

void kernel_doubleFaultEX(struct kernel_IRegs iregs){
	kernel_panic("Double fault exception");
}

void __attribute__((cdecl)) kernel_pagingEX(struct kernel_IRegs iregs){
	u32 eip = iregs.eip;
	u32 ebp = iregs.ebp;
	u32 eax = iregs.eax;
	kernel_printfBOCHS("EIP: %x\nEBP: %x\nEAX: %x", eip, ebp, eax);
	/*u32 error_code = iregs.error_code;
	u32 vaddr_cause = 0;
	__asm__ volatile ("mov eax, cr2"
					  : "=a" (vaddr_cause));

	const char* read_or_write = "read";

	if ((error_code & 2) == 2)
		read_or_write = "write";

	kernel_breakBOCHS();
	kernel_printfBOCHS("Paging Fault!!\nVaddr: %x\nError code: %x\nW/R: \"%s\"\nregs: %x\n", vaddr_cause, error_code, read_or_write, eax);
	kernel_breakBOCHS();*/
	kernel_panic("");
}