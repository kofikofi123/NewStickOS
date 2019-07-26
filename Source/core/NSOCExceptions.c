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
	kernel_printfBOCHS("OKR: %x\n", iregs.error_code);
	kernel_panic("Double fault exception");
}

void __attribute__((cdecl)) kernel_pagingEX(struct kernel_IRegs iregs){
	u32 eip = iregs.eip;
	u32 error_code = iregs.error_code;
	u32 vaddr_cause = 0;
	__asm__ volatile ("mov eax, cr2"
					  : "=a" (vaddr_cause));

	const char* read_or_write = "read";

	if ((error_code & 2) == 2)
		read_or_write = "write";

	kernel_printfBOCHS("\n\nPaging Fault!!\nVaddr: %x on %s\nError code: %x\neip: %x\n\n", vaddr_cause, read_or_write, error_code, eip);
	

	kernel_panic("");
}